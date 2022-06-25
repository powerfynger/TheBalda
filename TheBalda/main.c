/* Список возможных улучншений */

/* 1) Добавить глобальную структуру для игрового поля? */



#define _CRT_SECURE_NO_WARNINGS
#include "wincon.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>


#define  INV					"inv.txt"
#define	 LETTER_a_RUS			224
#define  DICT					"dict.txt"
#define  START_WORDS			"start_words.txt"
#define	 ALPHABET_POW			32
#define  MAX_WORD_LEN			30
#define  MAX_WORDS_COUNT		21
#define	 clr_bg					CON_CLR_BLACK
#define  clr_bg_active			CON_CLR_RED
#define  clr_font				CON_CLR_WHITE_LIGHT
#define  clr_bg_chosen			CON_CLR_GREEN
#define  clr_bg_warning         CON_CLR_YELLOW
#define  five_count             2236
#define  x_coord_field          80
#define  y_coord_field          5
#define  x_coord_menu           x_coord_field + 14
#define  MODE_DUEL				1
#define  MODE_VS_ROBOT			2
#define  MODE_DZEN				3


/*Структура узла словарного и инвертированного префиксных деревьев*/
typedef struct node {
	unsigned char letters[ALPHABET_POW];
	struct node* next[ALPHABET_POW];
	char* word;
}NODE;

//void demo_animation();
//void demo_colors();
//void demo_input();
void main_menu();
void about();
void settings_menu();
void difficulty_selection();
void first_turn_selection();
void set_letter();
int surrender_window();
void show_score();
void show_words_bank();
int set_word(int column_active_idx, int line_active_idx);
int search_letter(char letter, NODE* node);
void show_end_game(int is_sur);

char game_mode = MODE_VS_ROBOT;
char found = 0;
NODE* root_dict;
NODE* root_inv;
unsigned char* longest_word[MAX_WORD_LEN] = { '\0' };
int difficult = 1; //сложность изначально "лёгкий"
int start_turn = 1, turn = 0, turn_test = 1;
unsigned char words_bank[MAX_WORDS_COUNT][MAX_WORD_LEN];
int words_bank_len = 0;
int h_score, c_score;
char field_for_search[5][5] = { {1} }; // Нужно для отметок уже отработанных клеток для хода ИИ
int max_len = 0, x_start = 0, y_start = 0, x_chosen = 0, y_chosen = 0;
unsigned char letter_chosen = 0;
unsigned char field_letters[5][5] = { {'\0'} };
int left, top, btn_bg;

/*Поиск индекса буквы внутри узла*/
int find_node(unsigned char letter, NODE* node) {
	for (int i = 0; i < ALPHABET_POW; i++) {
		if (node->letters[i] == NULL) {
			return i;
		}
		if (node->letters[i] == letter) {
			return i;
		}
	}
}

/*Поиск индекса буквы внутри узла*/
int get_letter_index(unsigned char letter, NODE* node) {
	for (int i = 0; i < ALPHABET_POW; i++) {
		if (node->letters[i] == NULL) {
			return -1;
		}
		if (node->letters[i] == letter) {
			return i;
		}
	}
	return -1;
}

/*Вставка слова в дерево*/
void insert_word_tree(unsigned char* word, NODE* root) {
	NODE* node = root;
	for (int i = 0; i < strlen(word); i++) {
		int result = find_node(word[i], node);
		if (result != -1 && node->letters[result] == NULL) {
			node->letters[result] = word[i];
		}
		if (result != -1 && node->next[result] == NULL) {
			NODE* new_node = (NODE*)malloc(sizeof(NODE));
			if (new_node == NULL) {
				/*Нет доступной памяти*/
				return -1;
			}
			memset(new_node, 0, sizeof(NODE));
			new_node->word = NULL;
			node->next[result] = new_node;
		}
		node = node->next[result];
	}
	/*Вставка всего слова в поле word?*/
	//if (root != root_inv) {
	node->word = (char*)malloc(strlen(word));
	strcpy(node->word, word);
	//}
}

/*Считывание словаря в словарное дерево*/
void read_dict_to_tree(NODE* root) {
	FILE* file;
	char line[MAX_WORD_LEN + 2];
	int len = 0;
	if (fopen_s(&file, DICT, "r+")) {
		printf("Словарь не найден...");
		exit(EXIT_FAILURE);
	}
	while (fgets(line, sizeof(line), file) != NULL) {
		len = strlen(line) - 1;
		(line[len] == '\n') ? line[len] = '\0' : line[len];
		insert_word_tree(line, root);
	}
	fclose(file);
}

void read_inv_to_tree(NODE* root) {
	FILE* file;
	char line[MAX_WORD_LEN + 2];
	int len = 0;
	if (fopen_s(&file, INV, "r+")) {
		printf("Словарь не найден...");
		exit(EXIT_FAILURE);
	}
	while (fgets(line, sizeof(line), file) != NULL) {
		len = strlen(line) - 1;
		(line[len] == '\n') ? line[len] = '\0' : line[len];
		insert_word_tree(line, root);
	}
	fclose(file);
}

int find_word_tree(unsigned char* word, NODE* root) {
	NODE* node = root;
	for (int i = 0; i < strlen(word); i++) {
		for (int j = 0; j < ALPHABET_POW; j++) {
			if (node->letters[j] == NULL) {
				return 0;
			}
			if (node->letters[j] == word[i]) {
				node = node->next[j];
				break;
			}
		}
	}
	if (node->word != NULL) {
		return 1;
	}
	return 0;
}

/*int check valid -- непонятно зачем нужна*/

/*int search node -- существует точно такая же функция int get_letter_index() для поиска буквы в узле*/

/*Основная функция поиска в словарном дереве*/
void search_dict_tree(int x, int y, NODE* node, unsigned char* curr_word) {
	/*if (x >= 5 || y >= 5) return;
	if (x < 0 || y < 0) return;*/
	//if (x - 1 < 0 || y - 1 < 0 || x + 1 >= 5 || y + 1 >= 5) {
	//	return;
	//}
	if (difficult == 2 && strlen(curr_word) >= 3 && strlen(curr_word) < 6) {
		return;
	}
	int res = 0, checked = 0;
	//if (difficu/*lt == 2 && max_len >= 4) {
		//found = 1;
		//return;
	//}*/
	if (node->word != NULL && strlen(curr_word) > max_len) {
		int flag = 0;
		for (int i = 0; i < words_bank_len; i++) {
			if (!strncmp(words_bank[i], curr_word, MAX_WORD_LEN)) {
				flag = 1;
				break;
			}

		}
		if (!flag) {
			max_len = strlen(curr_word);
			for (int k = 0; curr_word[k] != '\0'; k++) {
				longest_word[k] = curr_word[k];
			}
			//strcpy(longest_word, curr_word);
			//if (difficult == 1) {
			//	found = 1;
			//	return;
			//}
			//if (difficult == 2 && max_len >= 4) {
			//	found = 1;
			//	return;
			//}
		}
	}
	if (difficult == 1 && strlen(curr_word) <= 4) {
		return;
	}
	if (field_for_search[x][y] == 0) {
		field_for_search[x][y] = 1;
		checked++;
	}
	// Проверяем все смежные клетки
	//Верхняя клетка
	res = get_letter_index(field_letters[x - 1][y], node);
	if (field_letters[x - 1][y] != '\0' && field_for_search[x - 1][y] != 1 && res != -1) {
		curr_word[strlen(curr_word)] = field_letters[x - 1][y];
		curr_word[strlen(curr_word)] = '\0';
		search_dict_tree(x - 1, y, node->next[res], curr_word);
		//if (found) return;
		curr_word[strlen(curr_word) - 1] = '\0';// Подумать над необходимостью
	}
	// Правая клетка
	res = get_letter_index(field_letters[x][y + 1], node);
	if (field_letters[x][y + 1] != '\0' && field_for_search[x][y + 1] != 1 && res != -1) {
		curr_word[strlen(curr_word)] = field_letters[x][y + 1];
		curr_word[strlen(curr_word)] = '\0';
		search_dict_tree(x, y + 1, node->next[res], curr_word);
		//if (found) return;
		curr_word[strlen(curr_word) - 1] = '\0';// Подумать над необходимостью
	}
	// Нижняя клетка
	res = get_letter_index(field_letters[x + 1][y], node);
	if (field_letters[x + 1][y] != '\0' && field_for_search[x + 1][y] != 1 && res != -1) {
		curr_word[strlen(curr_word)] = field_letters[x + 1][y];
		curr_word[strlen(curr_word)] = '\0';
		search_dict_tree(x + 1, y, node->next[res], curr_word);
		//if (found) return;
		curr_word[strlen(curr_word) - 1] = '\0';// Подумать над необходимостью
	}


	// Левая клетка
	res = get_letter_index(field_letters[x][y - 1], node);
	if (field_letters[x][y - 1] != '\0' && field_for_search[x][y - 1] != 1 && res != -1) {
		curr_word[strlen(curr_word)] = field_letters[x][y - 1];
		curr_word[strlen(curr_word)] = '\0';
		search_dict_tree(x, y - 1, node->next[res], curr_word);
		//if (found) return;
		curr_word[strlen(curr_word) - 1] = '\0';// Подумать над необходимостью
	}

	(checked == 1) ? field_for_search[x][y] = 0 : field_for_search[x][y];
	return;
}


/*Функция разворота слова(создание инвар.)*/
void reverse_word(unsigned char* word) {
	for (int i = 0, j = strlen(word) - 1; i < strlen(word) / 2; i++, j--) {
		unsigned char temp = word[i];
		word[i] = word[j];
		word[j] = temp;
	}
}


// Фукнция нужна для нахождения аналогичного узла из инвертированного дерева в словарном дереве 
// Который в дальнейшем используется в поиске по словарному дереву
NODE* inv_to_dict_node(unsigned char* word) {
	reverse_word(word);
	NODE* node = root_dict;
	for (int i = 0; i < strlen(word); i++) {
		for (int j = 0; j < ALPHABET_POW; j++) {
			if (node->letters[j] == word[i]) {
				node = node->next[j];
				break;
			}
		}
	}
	return node;
}

/*Основная функция поиска*/
void search_inv_tree(int x, int y, unsigned char* curr_word, int end_ind) {
	if (field_letters[x][y] == '\0' || x < 0 || y < 0 || x >= 5 || y >= 5) {
		return;
	}
	curr_word[end_ind] = field_letters[x][y];
	field_for_search[x][y] = 1;
	curr_word[end_ind + 1] = '\0';

	// Проверяем все смежные клетки
	unsigned char new_word[MAX_WORD_LEN] = { '\0' };
	strcpy(new_word, curr_word);
	if (find_word_tree(curr_word, root_inv)) {
		NODE* node = inv_to_dict_node(new_word);
		search_dict_tree(x_start, y_start, node, new_word);
		//if (found) return;
	}
	//Можно добавить оптимизацию
	/*
	else {

	}
	*/
	//Верхняя клетка
	if (field_letters[x - 1][y] != '\0' && field_for_search[x - 1][y] != 1) {
		search_inv_tree(x - 1, y, curr_word, end_ind + 1);
	}
	curr_word[end_ind + 1] = '\0';

	// Левая клетка
	if (field_letters[x][y + 1] != '\0' && field_for_search[x][y + 1] != 1) {
		search_inv_tree(x, y + 1, curr_word, end_ind + 1);
	}
	curr_word[end_ind + 1] = '\0';

	// Нижняя клетка
	if (field_letters[x + 1][y] != '\0' && field_for_search[x + 1][y] != 1) {
		search_inv_tree(x + 1, y, curr_word, end_ind + 1);
	}
	curr_word[end_ind + 1] = '\0';

	// Правая клетка
	if (field_letters[x][y - 1] != '\0' && field_for_search[x][y - 1] != 1) {
		search_inv_tree(x, y - 1, curr_word, end_ind + 1);
	}
	curr_word[end_ind + 1] = '\0';

	field_for_search[x][y] = 0;
}

void check_all_letters(int x, int y) {
	int curr_letter = LETTER_a_RUS;
	unsigned char curr_word[MAX_WORD_LEN] = { 0 };
	int end_ind = 0;
	for (int i = 0; i < ALPHABET_POW; i++) {
		int len = max_len;
		field_letters[x][y] = curr_letter;
		search_inv_tree(x, y, curr_word, end_ind);
		if (len < max_len) {
			x_chosen = x;
			y_chosen = y;
			letter_chosen = curr_letter;
		}
		field_letters[x][y] = '\0';
		memset(curr_word, 0, MAX_WORD_LEN);
		curr_letter++;
		//if (found) return;

	}
	return;
}


int bot_move() {
	// Стартовый индекс 1 или 0?
	int cell_count = 0;
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			if (field_letters[i][j] == '\0' && ((i + 1 < 5 && field_letters[i + 1][j] != '\0') ||
				(i - 1 >= 0 && field_letters[i - 1][j] != '\0') || (j + 1 < 5 && field_letters[i][j + 1] != '\0') || (j - 1 >= 0 && field_letters[i][j - 1] != '\0'))) {
				cell_count++;
				x_start = i;
				y_start = j;
				check_all_letters(i, j);
				//if (found) break;

			}
		}
		//if (found) break;
		//found = 0;
	}

	if (max_len == 0 && cell_count != 0) {
		printf("Слит ботяра");
		return 1;
	}
	else {
		field_letters[x_chosen][y_chosen] = letter_chosen;
		for (int i = 0; i < max_len; i++) {
			words_bank[words_bank_len][i] = longest_word[i];
		}
		words_bank_len += 1;
		c_score += max_len;

		max_len = 0;
	}
	return 0;
}

void init_dict_tree() {
	root_dict = (NODE*)malloc(sizeof(NODE));
	if (root_dict == NULL) {
		exit(EXIT_FAILURE);
	}
	memset(root_dict, 0, sizeof(NODE));
}

void init_inv_tree() {
	root_inv = (NODE*)malloc(sizeof(NODE));
	if (root_inv == NULL) {
		exit(EXIT_FAILURE);
	}
	memset(root_inv, 0, sizeof(NODE));
}

int main()
{
	system("chcp 1251");
	HWND hWnd = GetForegroundWindow();
	ShowWindow(hWnd, SW_MAXIMIZE);
	system("cls");
	FILE* file;
	init_dict_tree();
	init_inv_tree();
	read_dict_to_tree(root_dict);
	read_inv_to_tree(root_inv);
	//read_dict_to_tree()
	/*file = fopen(DICT, "r");
	if (file == NULL) {
		printf("I just don't have the words to describe the pain i feel!");
		return 1;
	}
	fclose(file);*/
	file = fopen(START_WORDS, "r");
	if (file == NULL) {
		printf("I just don't have the words with five letters to describe the pain i feel!");
		return 1;
	}
	fclose(file);
	// Инициализируется консоль, скрывается курсор, добавляется русский язык в консоль
	con_init(100, 50);
	// system("mode con cols=100 lines=25");
	show_cursor(0);
	h_score = 0, c_score = 0;
	// Запуск главного меню
	main_menu();

	return 0;
}


// Поддержка главного меню
void main_menu()
{
	const char* menu_items[] = { "Балда" ,"Игра", "Настройки", "Таблица рекордов", "О программе", "Выход" };
	int menu_active_idx = 1;
	int menu_items_count = sizeof(menu_items) / sizeof(menu_items[0]);
	/*short clr_bg = CON_CLR_BLACK;
	short clr_bg_active = CON_CLR_RED;
	short clr_font = CON_CLR_WHITE_LIGHT;*/
	left = x_coord_menu;
	int top = y_coord_field;
	int b;
	while (1)
	{
		left = x_coord_menu;
		top = y_coord_field;
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

void mode_selection() {
	char* menu_items[] = { "Режим" ,"Против игрока", "Против компьютера", "Дзен" ,"Назад или ESC" };
	int menu_active_idx = 1;
	int menu_items_count = sizeof(menu_items) / sizeof(menu_items[0]);
	//short clr_bg = CON_CLR_BLACK;
	//short clr_bg_active = CON_CLR_RED;
	//short clr_font = CON_CLR_WHITE_LIGHT;
	//short clr_bg_chosen = CON_CLR_RED_LIGHT;
	while (1)
	{
		left = x_coord_menu;
		top = y_coord_field;
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
			if (b == game_mode) {
				gotoxy(left - 4, top);
				printf("--->|                   ");
			}
			else {
				gotoxy(left, top);
				printf("|                   ");
			}

			gotoxy(left + 10 - strlen(menu_items[b]) / 2, top);
			printf("%s", menu_items[b]);
			con_set_color(clr_font, btn_bg);
			if (b == game_mode) {
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
					game_mode = MODE_DUEL;


				if (menu_active_idx == 2)//Средняя сложность
					game_mode = MODE_VS_ROBOT;

				if (menu_active_idx == 3)//Сложная
					game_mode = MODE_DZEN;
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
	int i, j;
	int is_word;
	for (i = 0; i < 5; i++) {
		for (j = 0; j < 5; j++) {
			field_letters[i][j] = '\0';
		}
	}
	if (start_turn == 1)turn = 1;
	else turn = 0;
	//Случайнок слово в начале
	FILE* five_file;
	char five_word[7];
	five_file = fopen(START_WORDS, "r");
	srand(time(NULL));
	int r = rand() % five_count;
	system("cls");
	for (i = 1; i != r; i++) fgets(five_word, 7, five_file);
	field_letters[2][0] = five_word[0];
	field_letters[2][1] = five_word[1];
	field_letters[2][2] = five_word[2];
	field_letters[2][3] = five_word[3];
	field_letters[2][4] = five_word[4];
	words_bank[0][0] = five_word[0];
	words_bank[0][1] = five_word[1];
	words_bank[0][2] = five_word[2];
	words_bank[0][3] = five_word[3];
	words_bank[0][4] = five_word[4];
	words_bank[0][5] = '\0';
	words_bank_len = 1;
	/*field_letters[2][0] = 'ж';
	field_letters[2][1] = 'у';
	field_letters[2][2] = 'ч';
	field_letters[2][3] = 'о';
	field_letters[2][4] = 'к';
	words_bank[0][0] = 'ж';
	words_bank[0][1] = 'у';
	words_bank[0][2] = 'ч';
	words_bank[0][3] = 'о';
	words_bank[0][4] = 'к';
	words_bank[0][5] = '\0';
	words_bank_len = 1;*/
	fclose(five_file);

	int column_active_idx = 0;
	int line_active_idx = 0;
	int field_letters_column_count = 5;
	int field_letters_line_count = 5;
	while (1)
	{
		if (check_end_game() == 1) return;
		if (turn % 2 == 0 && game_mode == MODE_VS_ROBOT) {
			bot_move();
			memset(longest_word, 0, sizeof(longest_word));
			max_len = 0;
			x_chosen = 0; y_chosen = 0; x_start = 0; y_start = 0;
			found = 0;
			turn++;
		}
		left = x_coord_field;
		top = y_coord_field;
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
				left = x_coord_field + j * 9;
				top = y_coord_field + i * 5;
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

				gotoxy(left + 4, top);
				field_letters[i][j] != '\0' ? printf("%c", field_letters[i][j] - ALPHABET_POW) : printf("%c", field_letters[i][j]);
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

		show_words_bank();

		show_score();

		// Данные подготовлены, вывести на экран
		con_draw_release();

		while (!key_is_pressed()) // Если пользователь нажимает кнопку
		{
			if (turn % 2 == 0 && game_mode == MODE_VS_ROBOT) {
				bot_move();
			}
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
			else if (code == KEY_ENTER) {
				if (field_letters[column_active_idx][line_active_idx] != '\0') break;
				if ((column_active_idx == 4 && field_letters[0][line_active_idx] == '\0'
					|| column_active_idx != 4 && field_letters[column_active_idx + 1][line_active_idx] == '\0')
					&& (column_active_idx == 0 && field_letters[4][line_active_idx] == '\0'
						|| column_active_idx != 0 && field_letters[column_active_idx - 1][line_active_idx] == '\0')
					&& (line_active_idx == 4 && field_letters[column_active_idx][0] == '\0'
						|| line_active_idx != 4 && field_letters[column_active_idx][line_active_idx + 1] == '\0')
					&& (line_active_idx == 0 && field_letters[column_active_idx][4] == '\0'
						|| line_active_idx != 0 && field_letters[column_active_idx][line_active_idx - 1] == '\0')) break;
				left = x_coord_field + line_active_idx * 9;
				top = y_coord_field + column_active_idx * 5;
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
				field_letters[column_active_idx][line_active_idx] != '\0' ? printf("%c", field_letters[column_active_idx][line_active_idx] - ALPHABET_POW) : printf("%c", field_letters[column_active_idx][line_active_idx]);
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
						field_letters[column_active_idx][line_active_idx] = code; //- ALPHABET_POW;;
					}
					else break;
					is_word = set_word(column_active_idx, line_active_idx);
					if (is_word == 1) field_letters[column_active_idx][line_active_idx] = '\0';
					break;
				}
				break;
			}
			else if (code == KEY_BACK) {
				int pass_ac = pass_turn_window();
				if (pass_ac == 1) {
					turn++;
					for (j = 0; j < MAX_WORD_LEN; j++) {
						words_bank[turn][j] = '\0';
					}
					words_bank_len++;
					break;
				}
			}
			else if (code == KEY_ESC) // ESC - выход
			{
				int sur_ac = surrender_window();
				if (sur_ac == 1) {
					show_end_game(1);
					for (i = 0; i < words_bank_len; i++) {
						for (j = 0; j < MAX_WORD_LEN; j++) {
							words_bank[i][j] = '\0';
						}
					}
					h_score = 0, c_score = 0;
					turn = 0;
					return;
				}
				break;
			}

			pause(40); // Небольная пауза (чтобы не загружать процессор)
		} // while (!key_is_pressed())


		// "Съедается" оставшийся ввод
		while (key_is_pressed())
			key_pressed_code();

	} // while(1)
}
int check_end_game() {
	int count = 0;
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			if (field_letters[i][j] != '\0') count++;
		}
	}
	if (count == 25) {
		show_end_game(0);
		for (int i = 0; i < words_bank_len; i++) {
			for (int j = 0; j < MAX_WORD_LEN; j++) {
				words_bank[i][j] = '\0';
			}
		}
		h_score = 0, c_score = 0;
		turn = 0;
		return 1;
	}
}

int set_word(int column_active_idx, int line_active_idx) {
	int i, j;
	char field_word[25][2];
	int word_length = 0;
	int column_letter_idx = column_active_idx;
	int line_letter_idx = line_active_idx;
	while (1)
	{
		left = x_coord_field;
		top = y_coord_field;
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
				left = x_coord_field + j * 9;
				top = y_coord_field + i * 5;
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
				field_letters[i][j] == '\0' ? printf("%c", field_letters[i][j]) : printf("%c", field_letters[i][j] - ALPHABET_POW);
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

		show_words_bank();

		show_score();
		// Данные подготовлены, вывести на экран
		con_draw_release();
		while (!key_is_pressed()) // Если пользователь нажимает кнопку
		{
			int code = key_pressed_code();
			if (code == 'w' || code == 'W' || code == (unsigned char)'ц' || code == (unsigned char)'Ц') // Если это стрелка вверх
			{
				if (word_length > 0) {
					if (column_active_idx > 0)
					{
						for (n = 0; n < word_length; n++) {
							if (column_active_idx - 1 == field_word[n][0] && line_active_idx == field_word[n][1]) flag = 1; //прошли уже эту клетку?
						}
						if (field_letters[column_active_idx - 1][line_active_idx] != '\0') {
							//Добавляем букву
							if (flag != 1) {
								column_active_idx--;
								field_word[word_length][0] = column_active_idx;
								field_word[word_length][1] = line_active_idx;
								word_length++;
							}
							//Оформляем возврат
							else {
								if (word_length > 1 && field_word[word_length - 2][0] == column_active_idx - 1) {
									column_active_idx--;
									word_length--;
									memset(field_word[word_length], 6, 2 * sizeof(char));
								}
							}
						}
					}
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
						if (field_letters[column_active_idx + 1][line_active_idx] != '\0') {
							//Добавляем букву
							if (flag != 1) {
								column_active_idx++;
								field_word[word_length][0] = column_active_idx;
								field_word[word_length][1] = line_active_idx;
								word_length++;
							}
							//Оформляем возврат
							else {
								if (word_length > 1 && field_word[word_length - 2][0] == column_active_idx + 1) {
									column_active_idx++;
									word_length--;
									memset(field_word[word_length], 6, 2 * sizeof(char));
								}
							}
						}
					}
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
						if (field_letters[column_active_idx][line_active_idx + 1] != '\0') {
							//Добавляем букву
							if (flag != 1) {
								line_active_idx++;
								field_word[word_length][0] = column_active_idx;
								field_word[word_length][1] = line_active_idx;
								word_length++;
							}
							//Оформляем возврат
							else {
								if (word_length > 1 && field_word[word_length - 2][1] == line_active_idx + 1) {
									line_active_idx++;
									word_length--;
									memset(field_word[word_length], 6, 2 * sizeof(char));
								}
							}
						}

					}
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
						if (field_letters[column_active_idx][line_active_idx - 1] != '\0') {
							//Добавляем букву
							if (flag != 1) {
								line_active_idx--;
								field_word[word_length][0] = column_active_idx;
								field_word[word_length][1] = line_active_idx;
								word_length++;
							}
							//Оформляем возврат
							else {
								if (word_length > 1 && field_word[word_length - 2][1] == line_active_idx - 1) {
									line_active_idx--;
									word_length--;
									memset(field_word[word_length], 6, 2 * sizeof(char));
								}
							}
						}
					}
					//добавляем букву в слово
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
						left = x_coord_field;
						top = y_coord_field;
						// Заблокировать отрисовку
						con_draw_lock();

						// Очистка экрана
						con_set_color(clr_font, clr_bg);
						clrscr();
						// Цикл отрисовывает кнопку
						for (i = 0; i < 5; i++)
						{
							for (j = 0; j < 5; j++) {
								left = x_coord_field + j * 9;
								top = y_coord_field + i * 5;
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
								field_letters[i][j] == '\0' ? printf("%c", field_letters[i][j]) : printf("%c", field_letters[i][j] - ALPHABET_POW);
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
						show_words_bank();

						show_score();
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
						FILE* file = fopen(DICT, "r+");
						unsigned char str[MAX_WORD_LEN];
						int flag_compare = 0;
						while (!feof(file))
						{
							fgets(str, 30, file);
							int compare_idx = 0;
							while (compare_idx < word_length) {
								if (field_letters[field_word[compare_idx][0]][field_word[compare_idx][1]] == str[compare_idx]) compare_idx++;
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
										break;
									}
								}
								if (flag_compare != 1) {
									for (int i = 0; i < word_length; i++) {
										words_bank[words_bank_len][i] = field_letters[field_word[i][0]][field_word[i][1]];
									}
									words_bank_len += 1;
									if (game_mode == MODE_DUEL)
										if (turn % 2 != 0)
											h_score += word_length;
										else
											c_score += word_length;
									else
										h_score += word_length;
									turn++;
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
			else if (code == KEY_BACK) {
				int pass_ac = pass_turn_window();
				if (pass_ac == 1) {
					turn++;
					for (j = 0; j < MAX_WORD_LEN; j++) {
						words_bank[turn][j] = '\0';
					}
					words_bank_len++;
					return 1;
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
void show_score() {
	left = x_coord_field - 30;
	top = y_coord_field - 3;
	btn_bg = clr_bg;
	con_set_color(clr_font, btn_bg);
	gotoxy(left, top);
	printf("%s", "Игрок 1   Игрок 2");
	left = x_coord_field - 28;
	top++;
	gotoxy(left, top);
	printf("%d", h_score);
	gotoxy(left + 10, top);
	printf("%d", c_score);
}

void show_end_game(int is_sur) {
	left = x_coord_menu;
	top = y_coord_field;
	btn_bg = clr_bg;
	con_draw_lock();
	clrscr();
	con_set_color(clr_font, btn_bg);
	if (is_sur == 1 && turn % 2 == 0 || h_score > c_score) {
		gotoxy(left + 1, top);
		printf("%s", "Игрок 1 победил!");
	}
	else if (is_sur == 1 && turn % 2 == 1 || h_score < c_score) {
		gotoxy(left + 4, top);
		printf("%s", "Игрок 2 победил!");
	}
	else {
		gotoxy(left + 9, top);
		printf("%s", "Ничья");
	}
	top++;
	gotoxy(left, top);
	printf("%s", "Игрок 1         Игрок 2");
	left = x_coord_menu + 2;
	top++;
	gotoxy(left, top);
	printf("%d", h_score);
	gotoxy(left + 16, top);
	printf("%d", c_score);
	con_draw_release();
	while (!key_is_pressed());
}

void show_words_bank() {
	int top_player = y_coord_field - 2, top_computer = y_coord_field - 2;
	left = x_coord_field + 50;
	top = y_coord_field;
	gotoxy(left, top);
	if (start_turn == 1)
		printf("        Игрок 1                 Игрок 2");
	else
		printf("        Игрок 2                 Игрок 1");
	for (int i = 0; i < words_bank_len; i++)
	{
		if (i == 0) {
			left = x_coord_field + 62;
			top = y_coord_field - 3;
		}
		else if (i % 2 == 1) {
			left = x_coord_field + 50;
			top_player += 3;
			top = top_player;
		}
		else {
			left = x_coord_field + 50 + 24;
			top_computer += 3;
			top = top_computer;
		}
		btn_bg = clr_bg;
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
}
void settings_menu()
{
	const char* menu_items[] = { "Настройки" ,"Сложность", "Режим", "Первый ход", "Назад или ESC" };
	int menu_active_idx = 1;
	int menu_items_count = sizeof(menu_items) / sizeof(menu_items[0]);
	/*short clr_bg = CON_CLR_BLACK;
	short clr_bg_active = CON_CLR_RED;
	short clr_font = CON_CLR_WHITE_LIGHT;*/
	while (1)
	{
		left = x_coord_menu;
		top = y_coord_field;
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
				if (menu_active_idx == 4) // Выбран последний пункт - это выход
					return;
				if (menu_active_idx == 2) {
					mode_selection();
				}

				if (menu_active_idx == 1)//Выбран пункт сложность
					difficulty_selection();

				if (menu_active_idx == 3)//Выбран пункт Первый ход
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

int surrender_window() {
	const char* menu_items[] = { "Да", "Нет" };
	int menu_active_idx = 1;
	int menu_items_count = sizeof(menu_items) / sizeof(menu_items[0]);
	while (1) {
		left = x_coord_menu;
		top = y_coord_field;
		int b;
		int code;

		// Заблокировать отрисовку
		con_draw_lock();

		// Очистка экрана
		con_set_color(clr_font, clr_bg);
		clrscr();
		// Цикл отрисовывает кнопку
		short btn_bg = clr_bg;
		gotoxy(left, top);
		printf("Вы уверены, что хотите сдаться?");
		top += 2;
		for (b = 0; b < 2; b++)
		{
			btn_bg = clr_bg; // По умолчанию фон кнопки - как фон экрана
			if (b == menu_active_idx)
				btn_bg = clr_bg_active; // Если кнопка активна - то рисуется другим цветом
			gotoxy(left, top);
			con_set_color(clr_font, btn_bg);
			printf("%s", menu_items[b]);
			left += 3;
		}
		con_draw_release();

		while (!key_is_pressed()) // Если пользователь нажимает кнопку
		{
			code = key_pressed_code();
			if (code == 'a' || code == 'A' || code == (unsigned char)'ф' || code == (unsigned char)'Ф')
			{
				if (menu_active_idx > 0)
				{
					menu_active_idx--;
					break;
				}
				else
				{
					menu_active_idx = 1;
					break;
				}
			}
			else if (code == 'd' || code == 'D' || code == (unsigned char)'в' || code == (unsigned char)'В')
			{
				if (menu_active_idx < 1)
				{
					menu_active_idx++;
					break;
				}
				else
				{
					menu_active_idx = 0;
					break;
				}
			}
			else if (code == KEY_ESC || code == 'q' || code == 'Q' ||
				code == (unsigned char)'й' || code == (unsigned char)'Й') // ESC или 'q' - выход
			{
				return 0;
			}
			else if (code == KEY_ENTER) // Нажата кнопка Enter
			{
				if (menu_active_idx == 1) // Выбран первый пункт - это выход
					return 0;

				if (menu_active_idx == 0)// Выбран второй пункт - это выход
					return 1;
			}


			pause(40); // Небольная пауза (чтобы не загружать процессор)
		} // while (!key_is_pressed())


		// "Съедается" оставшийся ввод
		while (key_is_pressed())
			key_pressed_code();

	}
}

int pass_turn_window() {
	const char* menu_items[] = { "Да", "Нет" };
	int menu_active_idx = 1;
	int menu_items_count = sizeof(menu_items) / sizeof(menu_items[0]);
	while (1) {
		left = x_coord_menu;
		top = y_coord_field;
		int b;
		int code;

		// Заблокировать отрисовку
		con_draw_lock();

		// Очистка экрана
		con_set_color(clr_font, clr_bg);
		clrscr();
		// Цикл отрисовывает кнопку
		short btn_bg = clr_bg;
		gotoxy(left, top);
		printf("Вы уверены, что хотите пропустить ход?");
		top += 2;
		for (b = 0; b < 2; b++)
		{
			btn_bg = clr_bg; // По умолчанию фон кнопки - как фон экрана
			if (b == menu_active_idx)
				btn_bg = clr_bg_active; // Если кнопка активна - то рисуется другим цветом
			gotoxy(left, top);
			con_set_color(clr_font, btn_bg);
			printf("%s", menu_items[b]);
			left += 3;
		}
		con_draw_release();

		while (!key_is_pressed()) // Если пользователь нажимает кнопку
		{
			code = key_pressed_code();
			if (code == 'a' || code == 'A' || code == (unsigned char)'ф' || code == (unsigned char)'Ф')
			{
				if (menu_active_idx > 0)
				{
					menu_active_idx--;
					break;
				}
				else
				{
					menu_active_idx = 1;
					break;
				}
			}
			else if (code == 'd' || code == 'D' || code == (unsigned char)'в' || code == (unsigned char)'В')
			{
				if (menu_active_idx < 1)
				{
					menu_active_idx++;
					break;
				}
				else
				{
					menu_active_idx = 0;
					break;
				}
			}
			else if (code == KEY_ESC || code == 'q' || code == 'Q' ||
				code == (unsigned char)'й' || code == (unsigned char)'Й') // ESC или 'q' - выход
			{
				return 0;
			}
			else if (code == KEY_ENTER) // Нажата кнопка Enter
			{
				if (menu_active_idx == 1) // Выбран первый пункт - это пропуск хода
					return 0;

				if (menu_active_idx == 0)
					return 1;
			}


			pause(40); // Небольная пауза (чтобы не загружать процессор)
		} // while (!key_is_pressed())


		// "Съедается" оставшийся ввод
		while (key_is_pressed())
			key_pressed_code();

	}
}

void difficulty_selection()
{
	char* menu_items[] = { "Сложность", "Средняя", "Лёгкая", "Сложная" ,"Назад или ESC" };
	int menu_active_idx = 1;
	int menu_items_count = sizeof(menu_items) / sizeof(menu_items[0]);
	//short clr_bg = CON_CLR_BLACK;
	//short clr_bg_active = CON_CLR_RED;
	//short clr_font = CON_CLR_WHITE_LIGHT;
	//short clr_bg_chosen = CON_CLR_RED_LIGHT;
	while (1)
	{
		left = x_coord_menu;
		top = y_coord_field;
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
				gotoxy(left - 4, top);
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

				if (menu_active_idx == 2)//Лёгкая сложность
					difficult = 2;

				if (menu_active_idx == 1)//Средняя сложность
					difficult = 1;

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
	const char* menu_items[] = { "Право первого хода" ,"Игрок 1", "Игрок 2" ,"Назад или ESC" };
	int menu_active_idx = 1;
	int menu_items_count = sizeof(menu_items) / sizeof(menu_items[0]);
	//short clr_bg = CON_CLR_BLACK;
	//short clr_bg_active = CON_CLR_RED;
	//short clr_font = CON_CLR_WHITE_LIGHT;
	while (1)
	{
		left = x_coord_menu;
		top = y_coord_field;
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
			if (b == start_turn) {
				gotoxy(left - 4, top);
				printf("--->|                   ");
			}
			else {
				gotoxy(left, top);
				printf("|                   ");
			}

			gotoxy(left + 10 - strlen(menu_items[b]) / 2, top);
			printf("%s", menu_items[b]);
			con_set_color(clr_font, btn_bg);
			if (b == start_turn) {
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
				if (menu_active_idx == 3) // Выбран последний пункт - это выход
					return;

				if (menu_active_idx == 1)//Человек ходит первым
					start_turn = 1;

				if (menu_active_idx == 2)//Компьютер ходит первым
					start_turn = 2;

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