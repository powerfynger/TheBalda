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
	// Инициализируется консоль, скрывается курсор
	con_init(80, 25);
	show_cursor(0);

	// Запуск главного меню
	main_menu();

	return 0;
}




// Поддержка главного меню
void main_menu()
{
	const char* menu_items[] = {"Балда" ,"Игра", "Настройки", "Таблица рекордов", "О программе", "Выход"};
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

		// Заблокировать отрисовку
		con_draw_lock();

		// Очистка экрана
		con_set_color(clr_font, clr_bg);
		clrscr();
		// Цикл отрисовывает кнопку
		for (b = 0; b < menu_items_count; b++)
		{
			short btn_bg = clr_bg; // По умолчанию фон кнопки - как фон экрана
			if (b == menu_active_idx)
				btn_bg = clr_bg_active; // Если кнопка активна - то рисуется другим цветом
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

		// Данные подготовлены, вывести на экран
		con_draw_release();


		while (!key_is_pressed()) // Если пользователь нажимает кнопку
		{
			int code = key_pressed_code();
			if (code == KEY_UP) // Если это стрелка вверх
			{
				// То переход к верхнему пункту (если это возможно)
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
			else if (code == KEY_DOWN) // Если стрелка вниз
			{
				// То переход к нижнему пункту (если это возможно)
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
				code == (unsigned char)'й' || code == (unsigned char)'Й') // ESC или 'q' - выход
			{
				return;
			}
			else if (code == KEY_ENTER) // Нажата кнопка Enter
			{
				if (menu_active_idx == menu_items_count - 1) // Выбран последний пункт - это выход
					return;

				if (menu_active_idx == 0)

				if (menu_active_idx == 1)

				if (menu_active_idx == 2)

				if (menu_active_idx == menu_items_count - 2)

				break;
			}


			pause(40); // Небольная пауза (чтобы не загружать процессор)
		} // while (!key_is_pressed())


		// "Съедается" оставшийся ввод
		while (key_is_pressed())
			key_pressed_code();

	} // while(1)
}

