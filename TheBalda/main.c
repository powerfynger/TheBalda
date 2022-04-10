#define _CRT_SECURE_NO_WARNINGS
#include "wincon.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define	 clr_bg					CON_CLR_BLACK
#define  clr_bg_active			CON_CLR_RED
#define  clr_font				CON_CLR_WHITE_LIGHT
#define clr_bg_chosen			CON_CLR_RED_LIGHT

void main_menu();
void demo_animation();
void demo_colors();
void demo_input();
void about();
void settings_menu();
void difficulty_selection();
void first_turn_selection();
void game();
int difficult = 1; //сложность изначально "лёгкий"
int turn = 1;




int main()
{
	// Инициализируется консоль, скрывается курсор
	con_init(100, 25);
	// system("mode con cols=100 lines=25");
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
	/*short clr_bg = CON_CLR_BLACK;
	short clr_bg_active = CON_CLR_RED;
	short clr_font = CON_CLR_WHITE_LIGHT;*/
	while (1)
	{
		int left = 50;
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
			if (b == 0)
				printf("~~~~~~~~~~~~~~~~~~~~");
			else
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
			if (b == 0)
				printf("~~~~~~~~~~~~~~~~~~~~");
			else
				printf("====================");
			top += 2;
		}

		// Данные подготовлены, вывести на экран
		con_draw_release();


		while (!key_is_pressed()) // Если пользователь нажимает кнопку
		{
			int code = key_pressed_code();
			if (code == 'w' || code == 'W' || code == (unsigned char)'ц' || code == (unsigned char)'Ц') // Если это стрелка вверх
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
			else if (code == 's' || code == 'S' || code == (unsigned char)'ы' || code == (unsigned char)'Ы') // Если стрелка вниз
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
				if (menu_active_idx == 5) // Выбран последний пункт - это выход
					return;

				//if (menu_active_idx == 0)

				if (menu_active_idx == 2) // Выбран пункт "настройки"
					settings_menu();
				if (menu_active_idx == 1)
					game();
				//if (menu_active_idx == 3)
				if (menu_active_idx == 4)
					about();

				break;
			}


			pause(40); // Небольная пауза (чтобы не загружать процессор)
		} // while (!key_is_pressed())


		// "Съедается" оставшийся ввод
		while (key_is_pressed())
			key_pressed_code();

	} // while(1)
}

void game() {
	char field_letters[5][5] = {'\0'};
	int column_active_idx = 0;
	int line_active_idx = 0;
	int field_letters_column_count = 5;
	int field_letters_line_count = 5;
	while (1)
	{
		int left = 40;
		int top = 2;
		int i, j;

		// Заблокировать отрисовку
		con_draw_lock();

		// Очистка экрана
		con_set_color(clr_font, clr_bg);
		clrscr();
		// Цикл отрисовывает кнопку
		for (i = 0; i < field_letters_column_count; i++)
		{
			for (j = 0; j < field_letters_line_count; j++) {
				left = 40 + j * 9;
				top = 2 + i * 5;
				short btn_bg = clr_bg; // По умолчанию фон кнопки - как фон экрана
				if (i == column_active_idx && j == line_active_idx)
					btn_bg = clr_bg_active; // Если кнопка активна - то рисуется другим цветом

				gotoxy(left, top);
				con_set_color(clr_font, btn_bg);

				printf("---------"); 
				top++;
				gotoxy(left, top);
				printf("|       |");
				top++;
				gotoxy(left, top);
				printf("|       ");

				gotoxy(left+4, top);
				printf("%c", field_letters[i][j]);
				//printf("А", field_letters[i][j]);

				gotoxy(left+8, top);
				printf("|");
				top++;

				gotoxy(left, top);
				printf("|       |");
				top++;
				gotoxy(left, top);
				printf("---------");
			}
		}

		// Данные подготовлены, вывести на экран
		con_draw_release();


		while (!key_is_pressed()) // Если пользователь нажимает кнопку
		{
			int code = key_pressed_code();
			if (code == 'w' || code == 'W' || code == (unsigned char)'ц' || code == (unsigned char)'Ц') // Если это стрелка вверх
			{
				// То переход к верхнему пункту (если это возможно)
				if (column_active_idx > 0)
				{
					column_active_idx--;
					break;
				}
				else
				{
					column_active_idx = 4;
					break;
				}
			}
			else if (code == 's' || code == 'S' || code == (unsigned char)'ы' || code == (unsigned char)'Ы') // Если стрелка вниз
			{
				// То переход к нижнему пункту (если это возможно)
				if (column_active_idx < 4)
				{
					column_active_idx++;
					break;
				}
				else
				{
					column_active_idx = 0;
					break;
				}
			}
			else if (code == 'd' || code == 'D' || code == (unsigned char)'в' || code == (unsigned char)'В') // Если стрелка вправо
			{
				// То переход к правому пункту (если это возможно)
				if (line_active_idx < 4)
				{
					line_active_idx++;
					break;
				}
				else
				{
					line_active_idx = 0;
					break;
				}
			}
			else if (code == 'a' || code == 'A' || code == (unsigned char)'ф' || code == (unsigned char)'Ф') // Если это стрелка влево
			{
				// То переход к левому пункту (если это возможно)
				if (line_active_idx > 0)
				{
					line_active_idx--;
					break;
				}
				else
				{
					line_active_idx = 4;
					break;
				}
			}
			else if(code == KEY_ENTER){
				while (key_is_pressed())
					key_pressed_code();
				while (!key_is_pressed()) {
					code = key_pressed_code();
					if (code >= (unsigned char)'А' && code <= (unsigned char)'Я') {
						field_letters[column_active_idx][line_active_idx] = code;
						break;
					}
					else if (code >= (unsigned char)'а' && code <= (unsigned char)'я') {
						field_letters[column_active_idx][line_active_idx] = code - 32;
						break;
					}
					else break;
				}
				break;
			}
			else if (code == KEY_ESC) // ESC или 'q' - выход
			{
				return;
			}

			pause(40); // Небольная пауза (чтобы не загружать процессор)
		} // while (!key_is_pressed())


		// "Съедается" оставшийся ввод
		while (key_is_pressed())
			key_pressed_code();

	} // while(1)
}

void settings_menu()
{
	const char* menu_items[] = { "Настройки" ,"Сложность", "Первый ход", "Назад или ESC"};
	int menu_active_idx = 1;
	int menu_items_count = sizeof(menu_items) / sizeof(menu_items[0]);
	/*short clr_bg = CON_CLR_BLACK;
	short clr_bg_active = CON_CLR_RED;
	short clr_font = CON_CLR_WHITE_LIGHT;*/
	while (1)
	{
		int left = 50;
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

			if (b == 0)
				printf("~~~~~~~~~~~~~~~~~~~~");
			else
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
			if (b == 0)
				printf("~~~~~~~~~~~~~~~~~~~~");
			else
				printf("====================");			
			top += 2;
		}

		// Данные подготовлены, вывести на экран
		con_draw_release();


		while (!key_is_pressed()) // Если пользователь нажимает кнопку
		{
			int code = key_pressed_code();
			
			if (code == 'w' || code == 'W' || code == (unsigned char)'ц' || code == (unsigned char)'Ц') // Если это стрелка вверх
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
			else if (code == 's' || code == 'S' || code == (unsigned char)'ы' || code == (unsigned char)'Ы') // Если стрелка вниз
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
				if (menu_active_idx == 3) // Выбран последний пункт - это выход
					return;

				if (menu_active_idx == 1)//Выбран пункт сложность
					difficulty_selection();

				if (menu_active_idx == 2)//Выбран пункт Первый ход
					first_turn_selection();
				
				break;
			}


			pause(40); // Небольная пауза (чтобы не загружать процессор)
		} // while (!key_is_pressed())


		// "Съедается" оставшийся ввод
		while (key_is_pressed())
			key_pressed_code();

	} // while(1)
}

void difficulty_selection()
{
	char* menu_items[] = { "Сложность" ,"Лёгкая", "Средняя", "Сложная" ,"Назад или ESC"};
	int menu_active_idx = 1;
	int menu_items_count = sizeof(menu_items) / sizeof(menu_items[0]);
	//short clr_bg = CON_CLR_BLACK;
	//short clr_bg_active = CON_CLR_RED;
	//short clr_font = CON_CLR_WHITE_LIGHT;
	//short clr_bg_chosen = CON_CLR_RED_LIGHT;
	while (1)
	{
		int left = 50;
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

			//if (b == difficult)
			//	btn_bg = clr_bg_chosen;//Кнопка не активна и это текущая сложность
			if (b == menu_active_idx)
				btn_bg = clr_bg_active; // Если кнопка активна - то рисуется другим цветом
			gotoxy(left, top);
			con_set_color(clr_font, btn_bg);

			if (b == 0)
				printf("~~~~~~~~~~~~~~~~~~~~");
			else
				printf("====================");			
			top++;
			if (b == difficult) {
				gotoxy(left-4, top);
				printf("--->|                   ");
			}
			else {
				gotoxy(left, top);
				printf("|                   ");
			}

			gotoxy(left + 10 - strlen(menu_items[b]) / 2, top);
			printf("%s", menu_items[b]);
			con_set_color(clr_font, btn_bg);
			if (b == difficult) {
				gotoxy(left + 19, top);
				printf("|<---");
			}
			else {
				gotoxy(left + 19, top);
				printf("|");
			}
			top++;
			gotoxy(left, top);
			if (b == 0)
				printf("~~~~~~~~~~~~~~~~~~~~");
			else
				printf("====================");			
			top += 2;
		}

		// Данные подготовлены, вывести на экран
		con_draw_release();


		while (!key_is_pressed()) // Если пользователь нажимает кнопку
		{
			int code = key_pressed_code();
			if (code == 'w' || code == 'W' || code == (unsigned char)'ц' || code == (unsigned char)'Ц') // Если это стрелка вверх
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
			else if (code == code == 's' || code == 'S' || code == (unsigned char)'ы' || code == (unsigned char)'Ы') // Если стрелка вниз
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
				if (menu_active_idx == 4) // Выбран последний пункт - это выход
					return;

				if (menu_active_idx == 1)//Лёгкая сложность
					difficult = 1;

				if (menu_active_idx == 2)//Средняя сложность
					difficult = 2;

				if (menu_active_idx == 3)//Сложная
					difficult = 3;
				break;
			}


			pause(40); // Небольная пауза (чтобы не загружать процессор)
		} // while (!key_is_pressed())


		// "Съедается" оставшийся ввод
		while (key_is_pressed())
			key_pressed_code();

	} // while(1)
}

void first_turn_selection() {
	const char* menu_items[] = { "Право первого хода" ,"Человек", "Компьютер" ,"Назад или ESC" };
	int menu_active_idx = 1;
	int menu_items_count = sizeof(menu_items) / sizeof(menu_items[0]);
	//short clr_bg = CON_CLR_BLACK;
	//short clr_bg_active = CON_CLR_RED;
	//short clr_font = CON_CLR_WHITE_LIGHT;
	while (1)
	{
		int left = 50;
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
			if (b == turn)
				btn_bg = clr_bg_chosen;//Кнопка не активна и это текущая сложность
			if (b == menu_active_idx)
				btn_bg = clr_bg_active; // Если кнопка активна - то рисуется другим цветом

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

		// Данные подготовлены, вывести на экран
		con_draw_release();


		while (!key_is_pressed()) // Если пользователь нажимает кнопку
		{
			int code = key_pressed_code();
			if (code == 'w' || code == 'W' || code == (unsigned char)'ц' || code == (unsigned char)'Ц') // Если это стрелка вверх
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
			else if (code == 's' || code == 'S' || code == (unsigned char)'ы' || code == (unsigned char)'Ы') // Если стрелка вниз
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
				if (menu_active_idx == 3) // Выбран последний пункт - это выход
					return;

				if (menu_active_idx == 1)//Человек ходит первым
					turn = 1;

				if (menu_active_idx == 2)//Компьютер ходит первым
					turn = 2;

				break;
			}


			pause(40); // Небольная пауза (чтобы не загружать процессор)
		} // while (!key_is_pressed())


		// "Съедается" оставшийся ввод
		while (key_is_pressed())
			key_pressed_code();

	} // while(1)
}

void about() {
	con_set_color(CON_CLR_WHITE_LIGHT, CON_CLR_BLACK);
	clrscr();

	gotoxy(8, 2);
	printf("О программе:");

	con_set_color(CON_CLR_GRAY, CON_CLR_BLACK);
	gotoxy(8, 3);
	printf("Данная программа является примером использования библиотеки wincon.\n\n");

	gotoxy(8, 4);
	printf("Для продолжения нажмите любую клавишу.");

	key_pressed_code();
	return;
}