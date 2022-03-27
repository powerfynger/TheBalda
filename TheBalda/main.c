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
void settings_menu();
void difficulty_selection();
void first_turn_selection();

int difficult = 1; //��������� ���������� "�����"
int turn = 1;


int main()
{
	// ���������������� �������, ���������� ������
	con_init(80, 95);
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
	short clr_bg = CON_CLR_BLACK;
	short clr_bg_active = CON_CLR_RED;
	short clr_font = CON_CLR_WHITE_LIGHT;
	while (1)
	{
		int left = 50;
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

			printf("====================");
			top++;
			gotoxy(left, top);
			printf("|                   ");

			gotoxy(left + 10 - strlen(menu_items[b]) / 2, top);
			printf("%s", menu_items[b]);

			con_set_color(clr_font, btn_bg);
			gotoxy(left + 19, top);
			printf("|");
			top++;
			gotoxy(left, top);
			printf("====================");
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
				if (menu_active_idx == 5) // ������ ��������� ����� - ��� �����
					return;

				//if (menu_active_idx == 0)

				if (menu_active_idx == 2) // ������ ����� "���������"
					settings_menu();
				//if (menu_active_idx == 1)

				//if (menu_active_idx == 3)

				break;
			}


			pause(40); // ��������� ����� (����� �� ��������� ���������)
		} // while (!key_is_pressed())


		// "���������" ���������� ����
		while (key_is_pressed())
			key_pressed_code();

	} // while(1)
}

void settings_menu()
{
	const char* menu_items[] = { "���������" ,"���������", "������ �����...", "���������"};
	int menu_active_idx = 1;
	int menu_items_count = sizeof(menu_items) / sizeof(menu_items[0]);
	short clr_bg = CON_CLR_BLACK;
	short clr_bg_active = CON_CLR_RED;
	short clr_font = CON_CLR_WHITE_LIGHT;
	while (1)
	{
		int left = 50;
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

			printf("====================");
			top++;
			gotoxy(left, top);
			printf("|                   ");

			gotoxy(left + 10 - strlen(menu_items[b]) / 2, top);
			printf("%s", menu_items[b]);

			con_set_color(clr_font, btn_bg);
			gotoxy(left + 19, top);
			printf("|");
			top++;
			gotoxy(left, top);
			printf("====================");
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
				if (menu_active_idx == 3) // ������ ��������� ����� - ��� �����
					return;

				if (menu_active_idx == 1)//������ ����� ���������
					difficulty_selection();

				if (menu_active_idx == 2)//������ ����� ������ ���
					first_turn_selection();
				
				break;
			}


			pause(40); // ��������� ����� (����� �� ��������� ���������)
		} // while (!key_is_pressed())


		// "���������" ���������� ����
		while (key_is_pressed())
			key_pressed_code();

	} // while(1)
}


void difficulty_selection()
{
	const char* menu_items[] = { "���������" ,"˸����", "�������", "�������" ,"���������"};
	int menu_active_idx = 1;
	int menu_items_count = sizeof(menu_items) / sizeof(menu_items[0]);
	short clr_bg = CON_CLR_BLACK;
	short clr_bg_active = CON_CLR_RED;
	short clr_font = CON_CLR_WHITE_LIGHT;
	short clr_bg_chosen = CON_CLR_RED_LIGHT;
	while (1)
	{
		int left = 50;
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
			if (b == difficult)
				btn_bg = clr_bg_chosen;//������ �� ������� � ��� ������� ���������
			if (b == menu_active_idx)
				btn_bg = clr_bg_active; // ���� ������ ������� - �� �������� ������ ������

			gotoxy(left, top);
			con_set_color(clr_font, btn_bg);

			printf("====================");
			top++;
			gotoxy(left, top);
			printf("|                   ");

			gotoxy(left + 10 - strlen(menu_items[b]) / 2, top);
			printf("%s", menu_items[b]);

			con_set_color(clr_font, btn_bg);
			gotoxy(left + 19, top);
			printf("|");
			top++;
			gotoxy(left, top);
			printf("====================");
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
				if (menu_active_idx == 4) // ������ ��������� ����� - ��� �����
					return;

				if (menu_active_idx == 1)//˸���� ���������
					difficult = 1;

				if (menu_active_idx == 2)//������� ���������
					difficult = 2;

				if (menu_active_idx == 3)//�������
					difficult = 3;
				break;
			}


			pause(40); // ��������� ����� (����� �� ��������� ���������)
		} // while (!key_is_pressed())


		// "���������" ���������� ����
		while (key_is_pressed())
			key_pressed_code();

	} // while(1)
}

void first_turn_selection() {
	const char* menu_items[] = { "������ �����..." ,"�������", "���������" ,"���������" };
	int menu_active_idx = 1;
	int menu_items_count = sizeof(menu_items) / sizeof(menu_items[0]);
	short clr_bg = CON_CLR_BLACK;
	short clr_bg_active = CON_CLR_RED;
	short clr_font = CON_CLR_WHITE_LIGHT;
	short clr_bg_chosen = CON_CLR_RED_LIGHT;
	while (1)
	{
		int left = 50;
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
			if (b == turn)
				btn_bg = clr_bg_chosen;//������ �� ������� � ��� ������� ���������
			if (b == menu_active_idx)
				btn_bg = clr_bg_active; // ���� ������ ������� - �� �������� ������ ������

			gotoxy(left, top);
			con_set_color(clr_font, btn_bg);

			printf("====================");
			top++;
			gotoxy(left, top);
			printf("|                   ");

			gotoxy(left + 10 - strlen(menu_items[b]) / 2, top);
			printf("%s", menu_items[b]);

			con_set_color(clr_font, btn_bg);
			gotoxy(left + 19, top);
			printf("|");
			top++;
			gotoxy(left, top);
			printf("====================");
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
				if (menu_active_idx == 3) // ������ ��������� ����� - ��� �����
					return;

				if (menu_active_idx == 1)//������� ����� ������
					turn = 1;

				if (menu_active_idx == 2)//��������� ����� ������
					turn = 2;

				break;
			}


			pause(40); // ��������� ����� (����� �� ��������� ���������)
		} // while (!key_is_pressed())


		// "���������" ���������� ����
		while (key_is_pressed())
			key_pressed_code();

	} // while(1)
}