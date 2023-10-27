#define WIN32_LEAN_AND_MEAN  
#define INITGUID
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
using namespace std;
#include <WinSock2.h>
#include <windows.h>  
#include <windowsx.h>
#include <stdio.h>

#include <d3d9.h>
#include <d3dx9tex.h>
#include "gpdumb1.h"
#include "Protocol.h"

#pragma comment (lib, "ws2_32.lib")

// DEFINES ////////////////////////////////////////////////

#define MAX(a,b)	((a)>(b))?(a):(b)
#define	MIN(a,b)	((a)<(b))?(a):(b)

// defines for windows 
#define WINDOW_CLASS_NAME L"WINXCLASS"  // class name

#define WINDOW_WIDTH    750
#define WINDOW_HEIGHT   800

#define TILE_COUNT			11

#define	BUF_SIZE					1024
#define	WM_SOCKET			WM_USER + 1

// PROTOTYPES /////////////////////////////////////////////

// game console
int Game_Init(void* parms = nullptr);
int Game_Shutdown(void* parms = nullptr);
int Game_Main(void* parms = nullptr);

// GLOBALS ////////////////////////////////////////////////

HWND main_window_handle = nullptr; // save the window handle
HINSTANCE main_instance = nullptr; // save the instance
char buffer[80];                // used to print text

// 플레이어 Unit
BOB			player;				
// 다른 플레이어
BOB         skelaton[MAX_USER];     
// NPC
BOB			npc[NUM_NPC];      

#define MAX_MARK 12
// 간격 표시자
BOB			g_Mark[MAX_MARK];

// 맵
BITMAP_IMAGE reactor;      
// 맵에 깔린 타일
BITMAP_IMAGE black_tile;
BITMAP_IMAGE white_tile;

// 타일 1개당 크기
#define TILE_WIDTH 65
#define UNIT_TEXTURE  0

SOCKET	g_mysocket;
WSABUF	send_wsabuf;
char 			send_buffer[BUF_SIZE];
WSABUF	recv_wsabuf;
char			recv_buffer[BUF_SIZE];
char			packet_buffer[BUF_SIZE];
DWORD	in_packet_size = 0;
int				saved_packet_size = 0;

int				g_myid;

int				g_left_x = 0;
int				g_top_y = 0;

void ProcessPacket(char* ptr)
{
	static bool first_time = true;
	switch (ptr[1])
	{
	case SC_LOGIN_OK:
		{
			SC_Packet_Login_OK* my_packet = reinterpret_cast<SC_Packet_Login_OK*>(ptr);
			g_myid = my_packet->id;
			break;
		}
		// 플레이어가 새로 들어옴
	case SC_PUT_PLAYER:
		{
			SC_Packet_Put_Player* my_packet = reinterpret_cast<SC_Packet_Put_Player*>(ptr);
			int playerID = my_packet->id;

			// 들어온 플레이어가 나라면,
			if (playerID == g_myid)
			{
				player.x = my_packet->x;
				player.y = my_packet->y;
				player.attr |= BOB_ATTR_VISIBLE;
			}
			// 다른 유저라면,
			else if (playerID < MAX_USER)
			{
				// 플레이어 좌표
				skelaton[playerID].x = my_packet->x;
				skelaton[playerID].y = my_packet->y;
				skelaton[playerID].attr |= BOB_ATTR_VISIBLE;
			}
			// NPC
			else
			{
				npc[playerID - NPC_ID_START].x = my_packet->x;
				npc[playerID - NPC_ID_START].y = my_packet->y;
				npc[playerID - NPC_ID_START].attr |= BOB_ATTR_VISIBLE;
			}
			break;
		}
	case SC_POS:
		{
			SC_Packet_Pos* my_packet = reinterpret_cast<SC_Packet_Pos*>(ptr);
			int playerID = my_packet->id;
			if (playerID == g_myid)
			{
				// 배경화면이 움직이도록 좌표 변경
				// g_left_x, y : 배경의 시작위치 x, y
				g_left_x = my_packet->x - 4;
				g_top_y = my_packet->y - 4;
				player.x = my_packet->x;
				player.y = my_packet->y;
			}
			else if (playerID < MAX_USER)
			{
				skelaton[playerID].x = my_packet->x;
				skelaton[playerID].y = my_packet->y;
			}
			else
			{
				npc[playerID - NPC_ID_START].x = my_packet->x;
				npc[playerID - NPC_ID_START].y = my_packet->y;
			}
		break;
		}
	// 플레이어가 나갔거나, 화면에서 안보일 때 사용.
	case SC_REMOVE_PLAYER:
		{
			SC_Packet_Remove_Player* my_packet = reinterpret_cast<SC_Packet_Remove_Player*>(ptr);
			int playerID = my_packet->id;

			if (playerID == g_myid)
				skelaton[playerID].attr &= ~BOB_ATTR_VISIBLE;
			else if (playerID < MAX_USER)
				skelaton[playerID].attr &= ~BOB_ATTR_VISIBLE;
			else
				npc[playerID - NPC_ID_START].attr &= ~BOB_ATTR_VISIBLE;
			break;
		}

	case SC_CHAT:
	{
		SC_Packet_Chat* my_packet = reinterpret_cast<SC_Packet_Chat *>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid) 
		{
			wcsncpy_s(player.message, my_packet->message, 256);
			player.message_time = GetTickCount();
		}

		else if (other_id < MAX_USER) 
		{
			wcsncpy_s(skelaton[other_id].message, my_packet->message, 256);
			skelaton[other_id].message_time = GetTickCount();
		}

		else 
		{
			wcsncpy_s(npc[other_id - NPC_ID_START].message, my_packet->message, 256);
			npc[other_id - NPC_ID_START].message_time = GetTickCount();
		}
		break;
	}
		break;

	default:
		cout << "Unknown PACKET type [" << ptr[1] << "]" << endl;
	}
}

void ReadPacket(SOCKET sock)
{
	DWORD iobyte, ioflag = 0;

	int ret = WSARecv(sock, &recv_wsabuf, 1, &iobyte, &ioflag, nullptr, nullptr);
	if (ret) 
	{
		int err_code = WSAGetLastError();
		cout << "Recv Error - " << err_code << endl;
	}

	BYTE *ptr = reinterpret_cast<BYTE *>(recv_buffer);

	while (0 != iobyte) 
	{
		if (0 == in_packet_size) 
			in_packet_size = ptr[0];
		if (iobyte + saved_packet_size >= in_packet_size) 
		{
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			ProcessPacket(packet_buffer);
			ptr += in_packet_size - saved_packet_size;
			iobyte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else 
		{
			memcpy(packet_buffer + saved_packet_size, ptr, iobyte);
			saved_packet_size += iobyte;
			iobyte = 0;
		}
	}
}

void clienterror()
{
	exit(-1);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	PAINTSTRUCT	ps;		 
	HDC					hdc;	 

	switch (msg)
	{
	case WM_KEYDOWN: 
		{
			int x = 0, y = 0;

			if (wparam == VK_RIGHT)	 
				x += 1;
			if (wparam == VK_LEFT)	
				x -= 1;
			if (wparam == VK_UP)	
				y -= 1;
			if (wparam == VK_DOWN)	
				y += 1;

			CS_Packet_Up* my_packet = reinterpret_cast<CS_Packet_Up*>(send_buffer);
			my_packet->size = sizeof(my_packet);
			send_wsabuf.len = sizeof(my_packet);

			DWORD iobyte;
			// VK_RIGHT 또는 VK_LEFT 키가 눌렸을 때
			if (x != 0)
			{
				if (x == 1) 
					my_packet->type = CS_RIGHT;
				else 
					my_packet->type = CS_LEFT;

				int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, nullptr, nullptr);
				if (ret) 
				{
					int error_code = WSAGetLastError();
					cout << "Error while sending packet - " << error_code;
				}
			}
			if (y != 0) 
			{
				if (y == 1) 
					my_packet->type = CS_DOWN;
				else 
					my_packet->type = CS_UP;
				WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, nullptr, nullptr);
			}
			break;
		}

	case WM_CREATE:
		return 0; 

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		return 0;
	
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	// Recv만 AsyncSelect 모델 사용
	case WM_SOCKET:
		{
			if (WSAGETSELECTERROR(lparam)) 
			{
				closesocket((SOCKET)wparam);
				clienterror();
				break;
			}

			switch (WSAGETSELECTEVENT(lparam)) 
			{
			case FD_READ:
				ReadPacket((SOCKET)wparam);
				break;
			case FD_CLOSE:
				closesocket((SOCKET)wparam);
				clienterror();
				break;
			}
		}

	default:
		break;
	} 

	return (DefWindowProc(hwnd, msg, wparam, lparam));
} 

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int ncmdshow)
{
	WNDCLASS winclass;	
	HWND	 hwnd;		
	MSG		 msg;		

	winclass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc = WindowProc;
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;
	winclass.hInstance = hinstance;
	winclass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	winclass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	winclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName = nullptr;
	winclass.lpszClassName = WINDOW_CLASS_NAME;

	if (!RegisterClass(&winclass))
		return(0);

	if (!(hwnd = CreateWindow(WINDOW_CLASS_NAME, // class
		L"Chess Client",	 // title
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0, 0,	   // x,y
		WINDOW_WIDTH,  // width
		WINDOW_HEIGHT, // height
		nullptr,	   // handle to parent 
		nullptr,	   // handle to menu
		hinstance,// instance
		nullptr)))	// creation parms
		return(0);

	main_window_handle = hwnd;
	main_instance = hinstance;

	// 게임 초기화
	Game_Init();

	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} 
		Game_Main();
	} 
	Game_Shutdown();

	return(msg.wParam);
} 

int Game_Init(void *parms)
{
	// this function is where you do all the initialization 
	// for your game

	// set up screen dimensions
	screen_width = WINDOW_WIDTH;
	screen_height = WINDOW_HEIGHT;
	screen_bpp = 32;

	// initialize directdraw
	DD_Init(screen_width, screen_height, screen_bpp);

	// create and load the reactor bitmap image
	Create_Bitmap32(&reactor, 0, 0, 531, 532);
	Create_Bitmap32(&black_tile, 0, 0, 531, 532);
	Create_Bitmap32(&white_tile, 0, 0, 531, 532);

	Load_Image_Bitmap32(&reactor, L"../Resource/Map.bmp", 0, 0, BITMAP_EXTRACT_MODE_ABS);
	Load_Image_Bitmap32(&black_tile, L"../Resource/Map.bmp", 0, 0, BITMAP_EXTRACT_MODE_ABS);

	black_tile.x = 69;
	black_tile.y = 5;
	// 타일 하나당 크기가 65
	black_tile.height = TILE_WIDTH;
	black_tile.width = TILE_WIDTH;

	Load_Image_Bitmap32(&white_tile, L"../Resource/Map.bmp", 0, 0, BITMAP_EXTRACT_MODE_ABS);
	white_tile.x = 5;
	white_tile.y = 5;
	white_tile.height = TILE_WIDTH;
	white_tile.width = TILE_WIDTH;

	// 간격 표시자 생성
	int mark_width = 65;
	int texture_id = 1;
	Load_Texture(L"../Resource/Flag.png", texture_id, mark_width, mark_width);

	for (int i = 0; i < MAX_MARK; ++i)
	{
		if (!Create_BOB32(&g_Mark[i], 0, 0, mark_width, mark_width, 1, BOB_ATTR_SINGLE_FRAME))
			return(0);
		Load_Frame_BOB32(&g_Mark[i], texture_id, 0, 0, 0, BITMAP_EXTRACT_MODE_CELL);

		Set_Pos_BOB32(&g_Mark[i], 0, 0);
	}

	// 플레이어와 상대 플레이어들 객체 생성
	Load_Texture(L"../Resource/Chess.png", UNIT_TEXTURE, 384, 64);

	if (!Create_BOB32(&player, 0, 0, 64, 64, 1, BOB_ATTR_SINGLE_FRAME)) return(0);
	Load_Frame_BOB32(&player, UNIT_TEXTURE, 0, 2, 0, BITMAP_EXTRACT_MODE_CELL);

	// set up stating state of skelaton
	Set_Animation_BOB32(&player, 0);
	Set_Anim_Speed_BOB32(&player, 4);
	Set_Vel_BOB32(&player, 0, 0);
	Set_Pos_BOB32(&player, 0, 0);

	// create skelaton bob
	for (int i = 0; i < MAX_USER; ++i) 
	{
		if (!Create_BOB32(&skelaton[i], 0, 0, 64, 64, 1, BOB_ATTR_SINGLE_FRAME))
			return(0);
		Load_Frame_BOB32(&skelaton[i], UNIT_TEXTURE, 0, 0, 0, BITMAP_EXTRACT_MODE_CELL);

		// set up stating state of skelaton
		Set_Animation_BOB32(&skelaton[i], 0);
		Set_Anim_Speed_BOB32(&skelaton[i], 4);
		Set_Vel_BOB32(&skelaton[i], 0, 0);
		Set_Pos_BOB32(&skelaton[i], 0, 0);
	}

	// create skelaton bob
	for (int i = 0; i < NUM_NPC; ++i) 
	{
		if (!Create_BOB32(&npc[i], 0, 0, 64, 64, 1, BOB_ATTR_SINGLE_FRAME))
			return 0;
		Load_Frame_BOB32(&npc[i], UNIT_TEXTURE, 0, 4, 0, BITMAP_EXTRACT_MODE_CELL);

		// set up stating state of skelaton
		Set_Animation_BOB32(&npc[i], 0);
		Set_Anim_Speed_BOB32(&npc[i], 4);
		Set_Vel_BOB32(&npc[i], 0, 0);
		Set_Pos_BOB32(&npc[i], 0, 0);
		// Set_ID(&npc[i], i);
	}

	 //set clipping rectangle to screen extents so mouse cursor
	 //doens't mess up at edges
	//RECT screen_rect = {0,0,screen_width,screen_height};
	//lpddclipper = DD_Attach_Clipper(lpddsback,1,&screen_rect);

	// hide the mouse
	//ShowCursor(FALSE);

	WSADATA	wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	g_mysocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, 0);

	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(SERVER_PORT);
	ServerAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	int Result = WSAConnect(g_mysocket, (sockaddr *)&ServerAddr, sizeof(ServerAddr), nullptr, nullptr, nullptr, nullptr);

	WSAAsyncSelect(g_mysocket, main_window_handle, WM_SOCKET, FD_CLOSE | FD_READ);

	send_wsabuf.buf = send_buffer;
	send_wsabuf.len = BUF_SIZE;
	recv_wsabuf.buf = recv_buffer;
	recv_wsabuf.len = BUF_SIZE;

	return 1;
} 

int Game_Shutdown(void *parms)
{
	// this function is where you shutdown your game and
	// release all resources that you allocated

	// kill the reactor
	Destroy_Bitmap32(&black_tile);
	Destroy_Bitmap32(&white_tile);
	Destroy_Bitmap32(&reactor);

	for (int i = 0; i < MAX_MARK; ++i)
		Destroy_BOB32(&g_Mark[i]);

	Destroy_BOB32(&player);
	// kill skelaton
	for (int i = 0; i < MAX_USER; ++i) 
		Destroy_BOB32(&skelaton[i]);

	for (int i = 0; i < NUM_NPC; ++i)
		Destroy_BOB32(&npc[i]);

	// shutdonw directdraw
	DD_Shutdown();

	WSACleanup();

	return 1;
}

int Game_Main(void *parms)
{
	// this is the workhorse of your game it will be called
	// continuously in real-time this is like main() in C
	// all the calls for you game go here!
	// check of user is trying to exit
	if (KEY_DOWN(VK_ESCAPE) || KEY_DOWN(VK_SPACE))
		PostMessage(main_window_handle, WM_DESTROY, 0, 0);

	// start the timing clock
	Start_Clock();

	// clear the drawing surface
	DD_Fill_Surface(D3DCOLOR_ARGB(255, 0, 0, 0));

	// get player input

	g_pd3dDevice->BeginScene();
	g_pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);

	// draw the background reactor image
	for (int i = 0; i < TILE_COUNT; ++i)
	{
		for (int j = 0; j < TILE_COUNT; ++j)
		{
			int tile_x = i + g_left_x;
			int tile_y = j + g_top_y;

			if ((tile_x < 0) || (tile_y < 0)) 
				continue;

			//if(j % 8 == 0)
			//	Draw_Bitmap32(&flag_tile, TILE_WIDTH * i + 7, TILE_WIDTH * j + 7);

			if (((tile_x >> 2) % 2) == ((tile_y >> 2) % 2))
			{
				Draw_Bitmap32(&white_tile, TILE_WIDTH * i + 7, TILE_WIDTH * j + 7);
			}
			else
				Draw_Bitmap32(&black_tile, TILE_WIDTH * i + 7, TILE_WIDTH * j + 7);
		}
	}
	//	Draw_Bitmap32(&reactor);
	g_pSprite->End();
	g_pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);

	// 간격 표시자 그리기
	for (int i = 0; i < MAX_MARK; ++i)
	{
		// 간격 표시자 위치
		g_Mark[i].x = 8 * i;
		g_Mark[i].y = 8 * i;
		g_Mark[i].attr |= BOB_ATTR_VISIBLE;
		Draw_BOB32(&g_Mark[i]);
	}

	// 다른 플레이어 그리기
	Draw_BOB32(&player);
	for (int i = 0; i<MAX_USER; ++i) 
		Draw_BOB32(&skelaton[i]);

	// NPC 그리기
	for (int i = NPC_ID_START; i < NUM_NPC; ++i) 
		Draw_BOB32(&npc[i]);

	// draw some text
	wchar_t text[300];
	wsprintf(text, L"MY POSITION (%3d, %3d)", player.x, player.y);
	Draw_Text_D3D(text, 10, screen_height - 64, D3DCOLOR_ARGB(255, 255, 255, 255));

	g_pSprite->End();
	g_pd3dDevice->EndScene();

	// flip the surfaces
	DD_Flip();

	// sync to 3o fps
	//Wait_Clock(30);
	return 1;
} 