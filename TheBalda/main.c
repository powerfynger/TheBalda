#define _CRT_SECURE_NO_WARNINGS
#include "wincon.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void main_menu();
void demo_animation();
void demo_colors();
void demo_input();
void about();

int main()
{
	// ���������������� �������, ���������� ������
	con_init(80, 25);
	show_cursor(0);

	// ������ �������� ����
	main_menu();

	return 0;
}




// ��������� �������� ����
void main_menu()
{
	const char* menu_items[] = {"�����" ,"����", "���������", "������� ��������", "� ���������", "�����"};
	int menu_active_idx = 1;
	int menu_items_count = sizeof(menu_items) / sizeof(menu_items[0]);
	short clr_bg = CON_CLR_YELLOW;
	short clr_bg_active = CON_CLR_MAGENTA_LIGHT;
	short clr_font = CON_CLR_WHITE_LIGHT;
	while (1)
	{
		int left = 30;
		int top = 2;
		int b;

		// ������������� ���������
		con_draw_lock();

		// ������� ������
		con_set_color(clr_font, clr_bg);
		clrscr();
		// ���� ������������ ������
		for (b = 0; b < menu_items_count; b++)
		{
			short btn_bg = clr_bg; // �� ��������� ��� ������ - ��� ��� ������
			if (b == menu_active_idx)
				btn_bg = clr_bg_active; // ���� ������ ������� - �� �������� ������ ������
			gotoxy(left, top);
			con_set_color(clr_font, btn_bg);

			printf("********************");
			top++;
			gotoxy(left, top);
			printf("*                   ");

			gotoxy(left + 10 - strlen(menu_items[b]) / 2, top);
			printf("%s", menu_items[b]);

			con_set_color(clr_font, btn_bg);
			gotoxy(left + 19, top);
			printf("*");
			top++;
			gotoxy(left, top);
			printf("********************");
			top += 2;
		}

		// ������ ������������, ������� �� �����
		con_draw_release();


		while (!key_is_pressed()) // ���� ������������ �������� ������
		{
			int code = key_pressed_code();
			if (code == KEY_UP) // ���� ��� ������� �����
			{
				// �� ������� � �������� ������ (���� ��� ��������)
				if (menu_active_idx > 1)
				{
					menu_active_idx--;
					break;
				}
				else
				{
					menu_active_idx = menu_items_count - 1;
					break;
				}
			}
			else if (code == KEY_DOWN) // ���� ������� ����
			{
				// �� ������� � ������� ������ (���� ��� ��������)
				if (menu_active_idx + 1 < menu_items_count)
				{
					menu_active_idx++;
					break;
				}
				else
				{
					menu_active_idx = 1;
					break;
				}
			}
			else if (code == KEY_ESC || code == 'q' || code == 'Q' ||
				code == (unsigned char)'�' || code == (unsigned char)'�') // ESC ��� 'q' - �����
			{
				return;
			}
			else if (code == KEY_ENTER) // ������ ������ Enter
			{
				if (menu_active_idx == menu_items_count - 1) // ������ ��������� ����� - ��� �����
					return;

				if (menu_active_idx == 0)

				if (menu_active_idx == 1)

				if (menu_active_idx == 2)

				if (menu_active_idx == menu_items_count - 2)

				break;
			}


			pause(40); // ��������� ����� (����� �� ��������� ���������)
		} // while (!key_is_pressed())


		// "���������" ���������� ����
		while (key_is_pressed())
			key_pressed_code();

	} // while(1)
}

