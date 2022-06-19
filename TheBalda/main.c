#define _CRT_SECURE_NO_WARNINGS
#include "wincon.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>


#define  DICT					"dict.txt"
#define  START_WORDS			"start_words.txt"
#define	 clr_bg					CON_CLR_BLACK
#define  clr_bg_active			CON_CLR_RED
#define  clr_font				CON_CLR_WHITE_LIGHT
#define  clr_bg_chosen			CON_CLR_GREEN
#define  clr_bg_warning         CON_CLR_YELLOW
#define  five_count             2236


void main_menu();
void demo_animation();
void demo_colors();
void demo_input();
void about();
void settings_menu();
void difficulty_selection();
void first_turn_selection();
void set_letter();
int set_word(char field_letters[5][5], int column_active_idx, int line_active_idx);
int difficult = 1; //сложность изначально "лёгкий"
int turn = 1;
char words_bank[21][31];
int words_bank_len = 0;
clock_t start_turn;
clock_t end_turn;


int main()
{
	system("chcp 1251");
	HWND hWnd = GetForegroundWindow();
	ShowWindow(hWnd, SW_MAXIMIZE);
	system("cls");
	FILE* file;
	file = fopen(DICT, "r");
	if (file == NULL) {
		printf("I just don't have the words to describe the pain i feel!");
		return 1;
	}
	fclose(file);
	// Инициализируется консоль, скрывается курсор
	con_init(100, 50);
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
					set_letter();
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

void set_letter() {
	char field_letters[5][5];
	int i, j;
	int is_word;
	for (i = 0; i < 5; i++) {
		for (j = 0; j < 5; j++) {
			field_letters[i][j] = '\0';
		}
	}

	//Случайнок слово в начале
	FILE* five_file;
	char five_word[7];
	five_file = fopen(START_WORDS, "r");
	srand(time(NULL));
	int r = rand() % five_count;
	system("cls");
	for (i = 1; i != r; i++) fgets(five_word, 7, five_file);
	field_letters[2][0] = five_word[0] - 32;
	field_letters[2][1] = five_word[1] - 32;
	field_letters[2][2] = five_word[2] - 32;
	field_letters[2][3] = five_word[3] - 32;
	field_letters[2][4] = five_word[4] - 32;
	words_bank[0][0] = five_word[0] - 32;
	words_bank[0][1] = five_word[1] - 32;
	words_bank[0][2] = five_word[2] - 32;
	words_bank[0][3] = five_word[3] - 32;
	words_bank[0][4] = five_word[4] - 32;
	words_bank[0][5] = '\0';
	words_bank_len = 1;
	fclose(five_file);

	int column_active_idx = 0;
	int line_active_idx = 0;
	int field_letters_column_count = 5;
	int field_letters_line_count = 5;
	while (1)
	{
		int left = 40;
		int top = 2;
		int i, j;
		short btn_bg;
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
				btn_bg = clr_bg; // По умолчанию фон кнопки - как фон экрана
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
		left = 90;
		top = 2;
		for (i = 0; i < words_bank_len; i++)
		{
			short btn_bg = clr_bg;
			gotoxy(left, top);
			con_set_color(clr_font, btn_bg);
			printf("-----------------------");
			top++;
			gotoxy(left, top);
			printf("|                   ");

			gotoxy(left + 12 - strlen(words_bank[i]) / 2, top);
			printf("%s", words_bank[i]);

			con_set_color(clr_font, btn_bg);
			gotoxy(left + 22, top);
			printf("|");
			top++;
			gotoxy(left, top);
			printf("-----------------------");
			top ++;
		}
		// Данные подготовлены, вывести на экран
		con_draw_release();

		start_turn = clock() / CLOCKS_PER_SEC;
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
				if (field_letters[column_active_idx][line_active_idx] != '\0') break;
				if (   (column_active_idx == 4 && field_letters[0][line_active_idx] == '\0'
					|| column_active_idx != 4 && field_letters[column_active_idx + 1][line_active_idx] == '\0')
					&& (column_active_idx == 0 && field_letters[4][line_active_idx] == '\0'
					|| column_active_idx != 0 && field_letters[column_active_idx - 1][line_active_idx] == '\0')
					&& (line_active_idx == 4 && field_letters[column_active_idx][0] == '\0'
					|| line_active_idx != 4 && field_letters[column_active_idx][line_active_idx + 1] == '\0')
					&& (line_active_idx == 0 && field_letters[column_active_idx][4] == '\0'
					|| line_active_idx != 0 && field_letters[column_active_idx][line_active_idx - 1] == '\0')) break;
				left = 40 + line_active_idx * 9;
				top = 2 + column_active_idx * 5;
				btn_bg = clr_bg_chosen;

				gotoxy(left, top);
				con_set_color(clr_font, btn_bg);

				printf("---------");
				top++;
				gotoxy(left, top);
				printf("|       |");
				top++;
				gotoxy(left, top);
				printf("|       ");

				gotoxy(left + 4, top);
				printf("%c", field_letters[column_active_idx][line_active_idx]);
				//printf("А", field_letters[i][j]);

				gotoxy(left + 8, top);
				printf("|");
				top++;

				gotoxy(left, top);
				printf("|       |");
				top++;
				gotoxy(left, top);
				printf("---------");

				while (key_is_pressed())
					key_pressed_code();
				while (!key_is_pressed()) {
					code = key_pressed_code();
					if (code >= (unsigned char)'А' && code <= (unsigned char)'Я') {
						field_letters[column_active_idx][line_active_idx] = code;
					}
					else if (code >= (unsigned char)'а' && code <= (unsigned char)'я') {
						field_letters[column_active_idx][line_active_idx] = code - 32;
					}
					else break;
					is_word = set_word(field_letters, column_active_idx, line_active_idx);
					if (is_word == 1) field_letters[column_active_idx][line_active_idx] = '\0';
					break;
				}
				break;
			}
			else if (code == KEY_ESC) // ESC - выход
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

int set_word(char field_letters[5][5], int column_active_idx, int line_active_idx) {
	int i, j;
	char field_word[25][2];
	int word_length = 0;
	int column_letter_idx = column_active_idx;
	int line_letter_idx = line_active_idx;
	while (1)
	{
		int left = 40;
		int top = 2;
		int i, j, k, n;
		short btn_bg;
		int flag = 0;
		// Заблокировать отрисовку
		con_draw_lock();

		// Очистка экрана
		con_set_color(clr_font, clr_bg);
		clrscr();
		// Цикл отрисовывает кнопку
		for (i = 0; i < 5; i++)
		{
			for (j = 0; j < 5; j++) {
				left = 40 + j * 9;
				top = 2 + i * 5;
				btn_bg = clr_bg; // По умолчанию фон кнопки - как фон экрана
				if (i == column_active_idx && j == line_active_idx)
					btn_bg = clr_bg_chosen; // Если кнопка активна - то рисуется другим цветом
				for (k = 0; k < word_length; k++) {
					if (i == field_word[k][0] && j == field_word[k][1]) {
						btn_bg = clr_bg_chosen;
					}
				}
				gotoxy(left, top);
				con_set_color(clr_font, btn_bg);

				printf("---------");
				top++;
				gotoxy(left, top);
				printf("|       |");
				top++;
				gotoxy(left, top);
				printf("|       ");

				gotoxy(left + 4, top);
				printf("%c", field_letters[i][j]);
				//printf("А", field_letters[i][j]);

				gotoxy(left + 8, top);
				printf("|");
				top++;

				gotoxy(left, top);
				printf("|       |");
				top++;
				gotoxy(left, top);
				printf("---------");
			}
		}
		left = 90;
		top = 2;
		for (i = 0; i < words_bank_len; i++)
		{
			short btn_bg = clr_bg;
			gotoxy(left, top);
			con_set_color(clr_font, btn_bg);
			printf("-----------------------");
			top++;
			gotoxy(left, top);
			printf("|                   ");

			gotoxy(left + 12 - strlen(words_bank[i]) / 2, top);
			printf("%s", words_bank[i]);

			con_set_color(clr_font, btn_bg);
			gotoxy(left + 22, top);
			printf("|");
			top++;
			gotoxy(left, top);
			printf("-----------------------");
			top++;
		}
		// Данные подготовлены, вывести на экран
		con_draw_release();

		while (!key_is_pressed()) // Если пользователь нажимает кнопку
		{
			end_turn = clock() / CLOCKS_PER_SEC;
			if (end_turn - start_turn >= 60) return 1;
			int code = key_pressed_code();
			if (code == 'w' || code == 'W' || code == (unsigned char)'ц' || code == (unsigned char)'Ц') // Если это стрелка вверх
			{
				if (word_length > 0) {
					if (column_active_idx > 0)
					{
						for (n = 0; n < word_length; n++) {
							if (column_active_idx - 1 == field_word[n][0] && line_active_idx == field_word[n][1]) flag = 1; //прошли уже эту клетку?
						}
						if (field_letters[column_active_idx - 1][line_active_idx] != '\0' && flag != 1) {
							column_active_idx--;
						}
					}
					//добавляем букву в слово
					word_length++;
					field_word[word_length - 1][0] = column_active_idx;
					field_word[word_length - 1][1] = line_active_idx;
				}
				else
				{
					if (column_active_idx > 0)
					{
						column_active_idx--;
					}
					else
					{
						column_active_idx = 4;
					}
				}
				break;
			}
			else if (code == 's' || code == 'S' || code == (unsigned char)'ы' || code == (unsigned char)'Ы') // Если стрелка вниз
			{

				if (word_length > 0) {
					if (column_active_idx < 4)
					{
						for (n = 0; n < word_length; n++) {
							if (column_active_idx + 1 == field_word[n][0] && line_active_idx == field_word[n][1]) flag = 1;//прошли уже эту клетку?
						}
						if (field_letters[column_active_idx + 1][line_active_idx] != '\0' && flag != 1) {
							column_active_idx++;
						}
					}
					//добавляем букву в слово
					word_length++;
					field_word[word_length - 1][0] = column_active_idx;
					field_word[word_length - 1][1] = line_active_idx;
				}
				else
				{
					if (column_active_idx < 4)
					{
						column_active_idx++;
					}
					else
					{
						column_active_idx = 0;
					}
				}
				break;
			}
			else if (code == 'd' || code == 'D' || code == (unsigned char)'в' || code == (unsigned char)'В') // Если стрелка вправо
			{
				if (word_length > 0) {
					if (line_active_idx < 4)
					{
						for (n = 0; n < word_length; n++) {
							if (line_active_idx + 1 == field_word[n][1] && column_active_idx == field_word[n][0]) flag = 1;//прошли уже эту клетку?
						}
						if (field_letters[column_active_idx][line_active_idx + 1] != '\0' && flag != 1) {
							line_active_idx++;
						}
					}
					//добавляем букву в слово
					word_length++;
					field_word[word_length - 1][0] = column_active_idx;
					field_word[word_length - 1][1] = line_active_idx;
				}
				else
				{
					if (line_active_idx < 4)
					{
						line_active_idx++;
					}
					else
					{
						line_active_idx = 0;
					}
				}
				break;
			}
			else if (code == 'a' || code == 'A' || code == (unsigned char)'ф' || code == (unsigned char)'Ф') // Если это стрелка влево
			{
				if (word_length > 0) {
					if (line_active_idx > 0)
					{
						for (n = 0; n < word_length; n++) {
							if (line_active_idx - 1 == field_word[n][1] && column_active_idx == field_word[n][0]) flag = 1;//прошли уже эту клетку?
						}
						if (field_letters[column_active_idx][line_active_idx - 1] != '\0' && flag != 1) {
							line_active_idx--;
						}
					}
					//добавляем букву в слово
					word_length++;
					field_word[word_length - 1][0] = column_active_idx;
					field_word[word_length - 1][1] = line_active_idx;
				}
				else
				{
					if (line_active_idx > 0)
					{
						line_active_idx--;
					}
					else
					{
						line_active_idx = 4;
					}
				}
				break;
			}
			else if (code == KEY_ENTER && field_letters[column_active_idx][line_active_idx] != '\0')
			{
				if (word_length == 0 && field_letters[column_active_idx][line_active_idx] != '\0') {
					word_length = 1;
					field_word[0][0] = column_active_idx;
					field_word[0][1] = line_active_idx;
				}
				else 
				{
					//Проверка: входит ли поставленная буква в выделенное слово
					flag = 0;
					for (n = 0; n < word_length; n++) {
						if (column_letter_idx == field_word[n][0] && line_letter_idx == field_word[n][1]) flag = 1;
					}
					if (flag == 0)
					{
						left = 40;
						top = 2;
						// Заблокировать отрисовку
						con_draw_lock();

						// Очистка экрана
						con_set_color(clr_font, clr_bg);
						clrscr();
						// Цикл отрисовывает кнопку
						for (i = 0; i < 5; i++)
						{
							for (j = 0; j < 5; j++) {
								left = 40 + j * 9;
								top = 2 + i * 5;
								btn_bg = clr_bg; // По умолчанию фон кнопки - как фон экрана
								if (i == column_letter_idx && j == line_letter_idx)
									btn_bg = clr_bg_warning; // Если это ячейка с не попавшей в слово буквой, то подсвечиваем жёлтым

								gotoxy(left, top);
								con_set_color(clr_font, btn_bg);

								printf("---------");
								top++;
								gotoxy(left, top);
								printf("|       |");
								top++;
								gotoxy(left, top);
								printf("|       ");

								gotoxy(left + 4, top);
								printf("%c", field_letters[i][j]);
								//printf("А", field_letters[i][j]);

								gotoxy(left + 8, top);
								printf("|");
								top++;

								gotoxy(left, top);
								printf("|       |");
								top++;
								gotoxy(left, top);
								printf("---------");
							}
						}
						left = 90;
						top = 2;
						for (i = 0; i < words_bank_len; i++)
						{
							short btn_bg = clr_bg;
							gotoxy(left, top);
							con_set_color(clr_font, btn_bg);
							printf("-----------------------");
							top++;
							gotoxy(left, top);
							printf("|                   ");

							gotoxy(left + 12 - strlen(words_bank[i]) / 2, top);
							printf("%s", words_bank[i]);

							con_set_color(clr_font, btn_bg);
							gotoxy(left + 22, top);
							printf("|");
							top++;
							gotoxy(left, top);
							printf("-----------------------");
							top++;
						}
						// Данные подготовлены, вывести на экран
						con_draw_release();

						word_length = 0;
						for (n = 0; n < word_length; n++) {
							field_word[n][0] = '\0';
							field_word[n][1] = '\0';
						}
						pause(100);
					}
					else 
					{
						//Проверка на наличие слова в словаре
						FILE* file = fopen(DICT, "r");
						char str[31];
						int flag_compare = 0;
						while (!feof(file))
						{
							fgets(str, 30, file);
							int compare_idx = 0;
							while (compare_idx < word_length) {
								if (field_letters[field_word[compare_idx][0]][field_word[compare_idx][1]] == str[compare_idx] - 32) compare_idx++;
								else break;
							}
							if (compare_idx == word_length && str[compare_idx] == '\n') {
								compare_idx = 0;
								for (int i = 0; i < words_bank_len; i++) {
									while (compare_idx < word_length) {
										if (field_letters[field_word[compare_idx][0]][field_word[compare_idx][1]] == words_bank[i][compare_idx]) compare_idx++;
										else break;
									}
									if (compare_idx == word_length && words_bank[i][compare_idx] == '\0') {
										flag_compare = 1;
									}
								}
								if (flag_compare != 1) {
									for (int i = 0; i < word_length; i++) {
										words_bank[words_bank_len][i] = field_letters[field_word[i][0]][field_word[i][1]];
									}
									words_bank_len += 1;
									return 0;
								}
							}
						}
						word_length = 0;
						for (n = 0; n < word_length; n++) {
							field_word[n][0] = '\0';
							field_word[n][1] = '\0';
						}

					}
				}
				break;
			}
			else if (code == KEY_ESC) // ESC - выход
			{
				return 1;
			}

			pause(40); // Небольная пауза (чтобы не загружать процессор)
		} // while (!key_is_pressed())


		// "Съедается" оставшийся ввод
		while (key_is_pressed())
			key_pressed_code();

	}
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
	printf("Что-то очём-то.\n\n");

	gotoxy(8, 4);
	printf("Для продолжения нажмите любую клавишу.");

	key_pressed_code();
	return;
}