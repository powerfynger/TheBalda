#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <Windows.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include "wincon_new.h"

#pragma warning (disable : 6386)

/*Ëîãè÷åñêèå ïåðåìåííûå*/
#define true_ 1
#define false_ 0
/*Îñíîâíîé ôîí*/
#define FONT CON_CLR_WHITE
/*Ïåðâîíà÷àëüíûé ñëîâàðü èç 11ê ñëîâ*/
#define dictionary "dictionary.txt" 
/*Âñå èíâåðòèðîâàííûå ïðåôèêñû, êîòîðûå åñòü ~45k*/
#define inversion "InvPrefix.txt"
/*Ïîëå, çàáëîêèðîâàííîå âñåãäà*/
#define BLOCKED 2
/*Ôàéë ñ ñîõðàíåííîé èãðîé*/
#define DOWNLOAD "Download.txt"
/*Ðåêîðäû*/
#define REC "Records.txt"

/*Êîîðäèíàòû*/
typedef struct map_
{
	short x;
	short y;
} map;

struct prefix_tree_;

/*ñòðóêòóðà, õðàíÿùàÿ ýëåìåíòû*/
typedef struct element_
{
	char letter;
	struct prefix_tree_* next;
} element;

/*Óçëû äåðåâà*/
typedef struct prefix_tree_
{
	/*Ìàêñèìàëüíî âîçìîæíûé ìàññèâ*/
	element current[33];
	/*Ïîêàçûâàåò êîíåö ëè ñëîâà*/
	enum
	{
		false,
		true
	} BOOL;
} prefix_tree;

typedef struct field_cell
{
	/*Áóêâà ÿ÷åéêè*/
	char letter;
	/*Ñòàòóñ ÿ÷åéêè*/
	enum
	{
		open,
		close
	} pass;
} cell;

/*Ñïèñîê óæå ïîñòàâëåííûõ ñëîâ*/
typedef struct Added
{
	char word[30];
	struct Added* next;
} added;

/*Îòñîðòèðîâàííîå äåðåâî äëÿ âûâîäà ðåêîðäîâ*/
typedef struct rec_
{
	char name[30];
	int points;
	struct rec_* left;
	struct rec_* right;
} vsOv;

prefix_tree* root, * root_inv;
added* list = NULL;
FILE* saved, * records;
int max_size = 0, start_x = 0, start_y = 0, lvl = 0, last_stand = 0, word_y = 3;
int from_x = 0, from_y = 0, check_end = 0, point_1 = 0, point_2 = 0, comp = 0, move = 0;
char longest[30], start_letter;

/*Ôóíêöèÿ äëÿ î÷èñòêè ïàìÿòè ïîñëå äåðåâüåâ*/
void free_tree(prefix_tree* node)
{
	for (int i = 0; i < 33; i++)
	{
		if (node->current[i].letter != NULL)
		{
			free_tree(node->current[i].next);
			continue;
		}
		free(node);
		break;
	}
}

/*Î÷èñòêà ïàìÿòè*/
void clear(cell** field)
{
	free(field);
	prefix_tree* node = root;
	free_tree(node);
	node = root_inv;
	free_tree(node);
}

/*Ïîèñê ñëîâà â ëèíåéíîì ñïèñêå*/
int search_in_list(char* word_)
{
	added* current = list;
	while (current != NULL)
	{
		/*Åñòü ñîâïàäåíèå*/
		if (strcmp(current->word, word_) == 0)
			return 0;
		current = current->next;
	}

	/*Íåò ñîâïàäåíèé*/
	return 1;
}

/*Äîáàâëåíèå â ëèñò òåêóùèõ ñëîâ*/
void add_to_list(char* word_)
{
	added* node = (added*)malloc(sizeof(added)), * current = list, * prev = NULL;
	if (node == NULL)
	{
		/*mistake here*/
		exit(1);
	}
	memset(node, 0, sizeof(added));
	node->next = NULL;
	strcpy(node->word, word_);

	if (current == NULL)
	{
		list = node;
		return;
	}

	while (current != NULL)
	{
		prev = current;
		current = current->next;
	}
	current = node;
	prev->next = current;
}

/*Âîçâðàùàåò èíäåêñ ýëåìåíòà âíóòðè óçëà*/
int search_in(char let, prefix_tree* node)
{
	for (int i = 0; i < 33; i++)
	{
		/*Áóêâà íå âñòðå÷àëàñü -> äîáàâèòü â íóëåâîé ýëåìåíò*/
		if (node->current[i].letter == NULL)
			return i;
		if (node->current[i].letter == let)
			return i;
	}
}

/*Ôóíêöèÿ äëÿ âñòàâêè ýëåìåíòîâ â äåðåâüÿ*/
void insert(char* word, prefix_tree* root)
{
	/*Òåêóùèé óçåë*/
	prefix_tree* node = root;
	for (int i = 0; i < strlen(word); i++)
	{
		int result = search_in(word[i], node);
		if (node->current[result].letter == NULL)
		{
			/*Åù¸ íå âñòðå÷àëàñü áóêâà íà äàííîì ýòàïå -> äîáàâëåíèå â ìàññèâ*/
			node->current[result].letter = word[i];
		}
		if (node->current[result].next == NULL)
		{
			prefix_tree* new_node = (prefix_tree*)malloc(sizeof(prefix_tree));
			if (new_node == NULL)
			{
				/*mistake here*/
				exit(1);
			}
			memset(new_node, 0, sizeof(prefix_tree));
			new_node->BOOL = false;
			node->current[result].next = new_node;
		}
		node = node->current[result].next;
	}
	node->BOOL = true;
}

/*Ñ÷èòûâàíèå ñëîâàðåé*/
void dict_read(prefix_tree* root, FILE* dict)
{
	/*Ïðîñòîå ñ÷èòûâàíèå ñëîâàðåé èç ôàéëîâ*/
	char ins_array[30];
	int i = 0;
	while (fgets(ins_array, 30, dict))
	{
		int size = strlen(ins_array) - 1;
		if (ins_array[size] == '\n')
			ins_array[size] = '\0';
		insert(ins_array, root);
	}
}

/*Îñíîâíîé ïîèñê ïî äåðåâüÿì -> 1 - ÷òî-òî íàøëîñü*/
int main_search(prefix_tree* root_, char* word)
{
	prefix_tree* node = root_;
	for (int i = 0; i < strlen(word); i++)
	{
		for (int j = 0; j < 33; j++)
		{
			if (node->current[j].letter == NULL)
			{
				/*Åñëè äîøëè äî ýëåìåíòà ñ ïàðàìåòðîì NULL - äàëüøå ñìîòðåòü íåò ñìûñëà*/
				return 0;
			}
			if (node->current[j].letter == word[i])
			{
				node = node->current[j].next;
				break;
			}
		}
	}
	/*Åñòü ñîâïàäåíèÿ*/
	if (node->BOOL == true)
		return 1;

	return 0;
}

/*Ïðîâåðêà ïðåôèêñà*/
int check_valid(char* new_current)
{
	/*Åñëè ó ïðåôèêñà íåò âåòêè â ñëîâàðå -> îí èçáûòî÷íûé*/
	prefix_tree* node = root_inv;
	for (int i = 0; i < strlen(new_current); i++)
	{
		for (int j = 0; j < 33; j++)
		{
			if (node->current[j].letter == new_current[i])
			{
				node = node->current[j].next;
				break;
			}
			if (j == 32)
				return 0;
		}
	}

	/*Óçåë öåëèêîì åñòü â äåðåâå ïðåôèêñîâ*/
	return 1;
}

/*Ïîèñê íóæíîé áóêâû â óçëå*/
int search_node(prefix_tree* node, char fnd)
{
	for (int i = 0; i < 33; i++)
	{
		if (node->current[i].letter == NULL)
			return -1;
		if (node->current[i].letter == fnd)
			return i;
	}

	return -1;
}

/*Äîñòðîåíèå ïðåôèêñîâ äî âîçìîæíûõ ñëîâ èç ãëàâíîãî ñëîâàðÿ*/
void dict_search(cell** field, int x, int y, prefix_tree* node, char* new_current)
{
	/*Åñëè óðîâåíü ñëîæíîñòè íèçêèé - âûáèðàþòñÿ ñëîâà ìåíüøå*/
	if (lvl < 3)
	{
		/*if (lvl == 1 && strlen(new_current) > 4)
			return;*/
		/*else */if (lvl == 2 && strlen(new_current) > 5)
			return;
	}

	/*Ïðîâåðêà òåêóùåãî ñëîâà íà îïòèìàëüíîñòü*/
	if (node->BOOL == true && strlen(new_current) > max_size)
	{
		if (search_in_list(new_current))
		{
			max_size = strlen(new_current);
			strcpy(longest, new_current);
		}
	}

	if (lvl == 1)
		return;

	/*Çàêðûòèå êëåòîê, êîòîðûå äî ýòîãî íå áûëè çàêðûòû*/
	int result = 0, cl = 0;
	if (field[x][y].pass == open)
	{
		field[x][y].pass = close;
		cl++;
	}

	/*Ðåêóðñèâíûé ïîèñê âîçìîæíûõ ñëîâ*/

	/*Ïðîâåðêà âåðõíåé êëåòêè*/
	result = search_node(node, field[x - 1][y].letter);
	if (field[x - 1][y].letter != ' ' && field[x - 1][y].pass != close && result != -1)
	{
		new_current[strlen(new_current)] = field[x - 1][y].letter;
		new_current[strlen(new_current)] = '\0';
		dict_search(field, x - 1, y, node->current[result].next, new_current);
		new_current[strlen(new_current) - 1] = '\0';
	}
	/*Ïðîâåðêà ïðàâîé êëåòêè*/
	result = search_node(node, field[x][y + 1].letter);
	if (field[x][y + 1].letter != ' ' && field[x][y + 1].pass != close && result != -1)
	{
		new_current[strlen(new_current)] = field[x][y + 1].letter;
		new_current[strlen(new_current)] = '\0';
		dict_search(field, x, y + 1, node->current[result].next, new_current);
		new_current[strlen(new_current) - 1] = '\0';
	}
	/*Ïðîâåðêà íèæíåé êëåòêè*/
	result = search_node(node, field[x + 1][y].letter);
	if (field[x + 1][y].letter != ' ' && field[x + 1][y].pass != close && result != -1)
	{
		new_current[strlen(new_current)] = field[x + 1][y].letter;
		new_current[strlen(new_current)] = '\0';
		dict_search(field, x + 1, y, node->current[result].next, new_current);
		new_current[strlen(new_current) - 1] = '\0';
	}
	/*Ïðîâåðêà ëåâîé êëåòêè*/
	result = search_node(node, field[x][y - 1].letter);
	if (field[x][y - 1].letter != ' ' && field[x][y - 1].pass != close && result != -1)
	{
		new_current[strlen(new_current)] = field[x][y - 1].letter;
		new_current[strlen(new_current)] = '\0';
		dict_search(field, x, y - 1, node->current[result].next, new_current);
		new_current[strlen(new_current) - 1] = '\0';
	}

	/*Åñëè âíóòðè ôóíêöèè áûëî çàêðûòî - îòêðûòü*/
	if (cl == 1)
		field[x][y].pass = open;

	return;
}

/*Ôóíêöèÿ ïåðåâîðà÷èâàåò ïðåôèêñ + ïðèâîäèò ê óçëó, ñ êîòîðîãî íåîáõîäèìî íà÷èíàòü*/
prefix_tree* create(char* new_current)
{
	/*Ðàçâîðîò òåêóùåãî ñëîâà äëÿ "äîïîèñêà" â îñíîâíîì ñëîâàðå*/
	int idx = strlen(new_current) - 1;
	for (int i = 0; i < strlen(new_current) / 2; i++, idx--)
	{
		char swap = new_current[i];
		new_current[i] = new_current[idx];
		new_current[idx] = swap;
	}

	/*Ïåðåõîä ê óçëó, ñ êîòîðîãî íåîáõîäèìî íà÷èíàòü "äîïîèñê"*/
	prefix_tree* node = root;
	for (int i = 0; i < strlen(new_current); i++)
	{
		for (int j = 0; j < 33; j++)
		{
			if (node->current[j].letter == new_current[i])
			{
				node = node->current[j].next;
				break;
			}
		}
	}

	/*Âîçâðàùàåò óçåë, ñ êîòîðîãî íóæíî íà÷èíàòü*/
	return node;
}

/*Ðåêóðñèâíûé ïîèñê ïðåôèêñîâ + îòïðàâêà íà äîñòðîåíèå â îñíîâíîì ñëîâàðå*/
void rec_prefix(cell** field, int x, int y, char* current, int idx)
{
	/*Åñëè äàííàÿ êëåòêà íåäîñòóïíà*/
	if (field[x][y].letter == ' ' || field[x][y].letter == BLOCKED)
		return;

	/*Äîáàâëåíèå òåêóùåãî ñëîâà + çàêðûòèå òåêóùåé ÿ÷åéêè*/
	current[idx] = field[x][y].letter;
	field[x][y].pass = close;
	current[idx + 1] = '\0';

	/*Íîâàÿ ïåðåìåííàÿ äëÿ ïåðåäà÷è â ðàçëè÷íûå ôóíêöèè*/
	char new_current[30];
	memset(new_current, 0, 30);
	strcpy(new_current, current);

	/*Ïðîâåðêà òåêóùåãî ñëîâà â äåðåâå ïðåôèêñîâ*/
	if (main_search(root_inv, current))
	{
		prefix_tree* node = create(new_current);
		/*Äîñòðîåíèå ñëîâàðÿ*/
		dict_search(field, from_x, from_y, node, new_current);
	}
	else
	{
		/*Ïðîâåðêà óçëà íà âîçìîæíûå äàëüíåéøèå ïîñòðîåíèÿ
		-> êîëîññàëüíîå ñîêðàùåíèå ðåêóðñèâíûõ âûçîâîâ*/
		if (!check_valid(new_current))
		{
			field[x][y].pass = open;
			return;
		}
	}


	/*Âåðõíÿÿ êëåòêà*/
	if (field[x - 1][y].letter != ' ' && field[x - 1][y].pass != close)
		rec_prefix(field, x - 1, y, current, idx + 1);
	current[idx + 1] = '\0';
	/*Ïðàâàÿ êëåòêà*/
	if (field[x][y + 1].letter != ' ' && field[x][y + 1].pass != close)
		rec_prefix(field, x, y + 1, current, idx + 1);
	current[idx + 1] = '\0';
	/*Íèæíÿÿ êëåòêà*/
	if (field[x + 1][y].letter != ' ' && field[x + 1][y].pass != close)
		rec_prefix(field, x + 1, y, current, idx + 1);
	current[idx + 1] = '\0';
	/*Ëåâàÿ êëåòêà*/
	if (field[x][y - 1].letter != ' ' && field[x][y - 1].pass != close)
		rec_prefix(field, x, y - 1, current, idx + 1);
	current[idx + 1] = '\0';

	/*Îòêðûòèå òåêóùåé êëåòêè*/
	field[x][y].pass = open;
}

/*Ïðîâåðêà âñåõ áóêâ*/
void find_prefix(cell** field, int x, int y) // field[][7]
{
	int start = 224;
	char current[30], idx = 0;
	memset(current, 0, 30);

	/*Ïðîâåðêà âñåõ áóêâ àëôàâèòà (êðîìå ¸)*/
	for (int i = 0; i < 32; i++)
	{
		int before = max_size;
		field[x][y].letter = start;
		rec_prefix(field, x, y, current, idx);
		/*Ìàêñèìàëüíûé ðàçìåð èçìåíèëñÿ*/
		if (before < max_size)
		{
			start_x = x;
			start_y = y;
			start_letter = start;
		}
		field[x][y].letter = ' ';
		memset(current, 0, 30);
		start++;
	}
}

/*Âûâîä òåêóùåãî ïîëÿ â êîíñîëü*/
void print_field(cell** field, int field_size)
{
	for (int i = 1; i < field_size + 1; i++)
	{
		for (int j = 1; j < field_size + 1; j++)
		{
			printf("%c", field[i][j].letter);
		}
		printf("\n");
	}
}

/*Óäàëåíèå òåêóùèõ ñëîâ*/
void clean_words()
{
	for (int i = 3; i < 26; i++)
	{
		gotoxy(11, i);
		for (int j = 0; j < 15; j++)
			fprintf(stdout, " ");
	}
	word_y = 3;
}

/*Çàíåñåíèå ðåêîðäà*/
void recV()
{
	records = fopen(REC, "a");
	clrscr();
	con_draw_lock();

	int x = con_width_new(), y = con_height_new() / 2 - 7;
	gotoxy((x - strlen("Ïîçäðàâëÿåì! Âû âûèðàëè, íàáðàâ 100 áàëëîâ")) / 2, y);
	fprintf(stdout, "Ïîçäðàâëÿåì! Âû âûèðàëè, íàáðàâ %d áàëëîâ!", point_1);
	gotoxy((x - strlen("Ïîçäðàâëÿåì! Âû âûèðàëè, íàáðàâ 100 áàëëîâ")) / 2, y + 2);
	con_draw_release();

	fprintf(stdout, "Ââåäèòå ñâîå èìÿ: ");
	char name[30];
	if (name != NULL)
		memset(name, 0, 30);
	else
		exit(1);

	show_cursor(1);
	if (fscanf(stdin, "%25s", name) != 1)
		exit(1);
	fprintf(records, "%d %s\n", point_1, name);
	show_cursor(0);
	pause(3000);
	fclose(records);
}

/*Õîä áîòà*/
int mrOvechkin_move(cell** field, int size)
{
	int start_check = 0;
	for (int i = 1; i < size + 1; i++)
	{
		for (int j = 1; j < size + 1; j++)
		{
			/*Ïîèñê êëåòêè, â êîòîðîé ìîæíî íà÷èíàòü ïåðåáîð*/
			if (field[i][j].letter == ' ' && (field[i + 1][j].letter < 0 ||
				field[i - 1][j].letter < 0 || field[i][j + 1].letter < 0 || field[i][j - 1].letter < 0))
			{
				start_check++;
				from_x = i;
				from_y = j;
				find_prefix(field, i, j);
			}
		}
	}

	if (max_size == 0)
	{
		check_end = 1;
		if (start_check != 0)
		{
			/*Äîáàâëåíèå â òàáëèöó ðåêîðäîâ*/
			recV();
			return 1;
		}
	}
	else
	{
		field[start_x][start_y].letter = start_letter;
		gotoxy((con_width_new() - size * 5) / 2 + 6 * (start_y - 1) + 2,
			(con_height_new() - size * 3) / 2 + 3 * (start_x - 1) + 1);
		fprintf(stdout, "%c", start_letter);
		point_2 += strlen(longest);
		gotoxy(4, 1);
		if (point_1 / 10 < 1)
			fprintf(stdout, "    %d    |     %d     ", point_1, point_2);
		else if (point_1 / 100 < 1)
			fprintf(stdout, "    %d   |     %d     ", point_1, point_2);
		else if (point_1 / 1000 < 1)
			fprintf(stdout, "    %d  |     %d     ", point_1, point_2);
		if (word_y < 25)
		{
			gotoxy(11, word_y++);
			fprintf(stdout, longest);
		}
		else
		{
			clean_words();
		}
		add_to_list(longest);

		max_size = 0;
	}
	return 0;
}

/*Îòêðûòèå ôàéëîâ*/
void create_FILES(FILE** dict, FILE** inv)
{
	if (((*dict) = fopen(dictionary, "r+")) == NULL || ((*inv) = fopen(inversion, "r+")) == NULL)
	{
		/*mistake here*/
		exit(1);
	}
}

/*Ñîçäàíèå äåðåâüåâ-ñëîâàðåé*/
void create_TRIES()
{
	root = (prefix_tree*)malloc(sizeof(prefix_tree));
	if (root == NULL)
	{
		/*mistake here*/
		exit(1);
	}
	memset(root, 0, sizeof(prefix_tree));

	root_inv = (prefix_tree*)malloc(sizeof(prefix_tree));
	if (root_inv == NULL)
	{
		/*mistake here*/
		exit(1);
	}
	memset(root_inv, 0, sizeof(prefix_tree));
}

/*Ñîçäàíèå ìàòðèöû-ïîëÿ*/
cell** create_FIELD(int* field_size)
{
	/*Îáíóëåíèå ãëîáàëüíîãî ìàññèâà*/
	memset(longest, 0, 30);

	cell** field = (cell**)malloc((*field_size + 2) * sizeof(cell*));
	memset(field, 0, (*field_size + 2) * sizeof(cell*));
	if (field != NULL)
	{
		for (int i = 0; i < *field_size + 2; i++)
		{
			field[i] = (cell*)malloc((*field_size + 2) * sizeof(cell));

			if (field[i] != NULL)
			{
				memset(field[i], 0, (*field_size + 2) * sizeof(cell));
			}
		}
	}

	for (int i = 1; i < *field_size + 1; i++)
	{
		for (int j = 1; j < *field_size + 1; j++)
			field[i][j].letter = ' ';
	}

	for (int i = 0; i < *field_size + 2; i++)
	{
		field[0][i].letter = BLOCKED;
		field[i][0].letter = BLOCKED;
		field[*field_size + 1][i].letter = BLOCKED;
		field[i][*field_size + 1].letter = BLOCKED;
	}

	///*Íà÷àëüíîå ñëîâî*/
	char* start_word = (char*)malloc(*field_size + 10);
	if (start_word != NULL)
		memset(start_word, 0, *field_size + 10);
	else
		exit(1);

	if (*field_size == 5)
		strcpy(start_word, "áàëäà");
	if (*field_size == 7)
		strcpy(start_word, "òðàêòîð");
	if (*field_size == 10)
		strcpy(start_word, "ìàðìåëàäêà");

	/*Åñëè íå÷åòíîå êîëè÷åñòâî - äåôîëòíîå ïîëîæåíèå*/
	if (*field_size % 2)
	{
		for (int i = 1; i < *field_size + 1; i++)
			field[*field_size / 2 + 1][i].letter = start_word[i - 1];
	}
	/*Åñëè ÷åòíîå - äèàãîíàëü*/
	else
	{
		for (int i = 1; i < *field_size + 1; i++)
			field[i][i].letter = start_word[i - 1];
	}

	return field;
}

/*Óäàëåíèå âûäåëåííûõ êëåòîê*/
void delete_red(map* selected, int idx, int size, cell** field)
{
	con_draw_lock();

	for (int i = 0; i < idx; i++)
	{
		gotoxy((con_width_new() - size * 5) / 2 + 6 * selected[i].x + 2,
			(con_height_new() - size * 3) / 2 + 3 * selected[i].y + 1);
		fprintf(stdout, "%c", field[selected[i].y + 1][selected[i].x + 1].letter);
	}

	con_draw_release();
}

/*Îòðèñîâêà îêíà ñ ìåíþ*/
void main_menu(char** vars, short active_button)
{
	clrscr();
	con_draw_lock();

	int x = con_width_new(), y = con_height_new() / 2 - 7, line = y - 1, line_2 = (con_width_new() - strlen(vars[4])) / 2 - 8;

	for (int i = 0; i < (16 + strlen(vars[4])) / 2 + 1; i++, line_2 += 2)
	{
		gotoxy(line_2, line - 1);
		fprintf(stdout, "#");
	}

	for (int i = 0; i < 13; i++, line++)
	{
		gotoxy((con_width_new() - strlen(vars[4])) / 2 - 8, line);
		fprintf(stdout, "#");
		gotoxy((con_width_new() + strlen(vars[4])) / 2 + 7, line);
		fprintf(stdout, "#");
	}

	line_2 = (con_width_new() - strlen(vars[4])) / 2 - 8;
	for (int i = 0; i < (16 + strlen(vars[4])) / 2 + 1; i++, line_2 += 2)
	{
		gotoxy(line_2, line);
		fprintf(stdout, "#");
	}

	for (int i = 0; i < 6; i++, y += 2)
	{
		gotoxy((x - strlen(vars[i])) / 2, y);
		if (i == active_button)
		{
			con_set_color(FONT, CON_CLR_GRAY);
			fprintf(stdout, "%s", vars[i]);
			con_set_color(FONT, CON_CLR_BLACK);
			continue;
		}
		fprintf(stdout, "%s", vars[i]);
	}
	con_draw_release();
}

/*Èçìåíåíèå àêòèâíîé êíîïêè*/
void changer(int active_button, char** vars, int colour)
{
	if (colour == 0)
	{
		con_set_color(FONT, CON_CLR_BLACK);
	}
	else
	{
		con_set_color(FONT, CON_CLR_GRAY);
	}

	for (int i = 0; i < strlen(vars[active_button]); i++)
	{
		gotoxy((con_width_new() - strlen(vars[active_button])) / 2 + i, con_height_new() / 2 - 7 + 2 * active_button);
		fprintf(stdout, " ");
	}

	gotoxy((con_width_new() - strlen(vars[active_button])) / 2, con_height_new() / 2 - 7 + 2 * active_button);
	fprintf(stdout, "%s", vars[active_button]);

	if (colour == 1)
	{
		con_set_color(FONT, CON_CLR_BLACK);
	}
}

/*Î ïðîãðàììå*/
void about()
{
	clrscr();
	con_draw_lock();

	char* vars[] = { "Òåêñò", "Òåêñò", "Òåêñò",
		"Òåêñò",
		"Òåêñò", "Äëÿ âûõîäà íàæìèòå ESC" };

	int x = con_width_new(), y = con_height_new() / 2 - 7;
	for (int i = 0; i < 5; i++, y += 2)
	{
		gotoxy((x - strlen(vars[i])) / 2, y);
		fprintf(stdout, "%s", vars[i]);
	}

	gotoxy(x - strlen(vars[5]) - 2, con_height_new() - 2);
	fprintf(stdout, "%s", vars[5]);

	con_draw_release();

	while (true_)
	{
		while (!key_is_pressed())
		{
			short code = key_pressed_code();
			if (code == KEY_ESC)
				return;
		}
		while (key_is_pressed())
			key_pressed_code();
	}
}

/*Çàãðóçêà èëè íîâàÿ èãðà*/
void menu_game(int active_button, char** vars)
{
	clrscr();
	con_draw_lock();

	int x = con_width_new(), y = con_height_new() / 2 - 7;
	for (int i = 0; i < 2; i++, y += 2)
	{
		gotoxy((x - strlen(vars[i])) / 2, y);
		if (i == active_button)
		{
			con_set_color(FONT, CON_CLR_GRAY);
			fprintf(stdout, "%s", vars[i]);
			con_set_color(FONT, CON_CLR_BLACK);
			continue;
		}
		fprintf(stdout, "%s", vars[i]);
	}

	gotoxy(x - strlen("Äëÿ âûõîäà íàæìèòå ESC") - 2, con_height_new() - 2);
	fprintf(stdout, "Äëÿ âûõîäà íàæìèòå ESC");
	con_draw_release();
}

/*Âûáîð ïàðàìåòðîâ*/
void change_lvl(short active, char** vars, int pl, int pl_1)
{
	pl_1 = pl_1 == 0 ? 5 : pl_1;
	pl_1 = pl_1 == 2 ? -3 : pl_1;
	if (pl)
	{
		for (int i = 0; i < strlen(vars[active - 1]); i++)
		{
			gotoxy((con_width_new() - strlen(vars[active - 1])) / 2 + i, con_height_new() / 2 - pl_1);
			fprintf(stdout, " ");
		}
	}
	else
	{
		for (int i = 0; i < strlen(vars[active + 1]); i++)
		{
			gotoxy((con_width_new() - strlen(vars[active + 1])) / 2 + i, con_height_new() / 2 - pl_1);
			fprintf(stdout, " ");
		}
	}
	gotoxy((con_width_new() - strlen(vars[active])) / 2, con_height_new() / 2 - pl_1);
	fprintf(stdout, "%s", vars[active]);
}

/*Ïåðåìåøåíèå ÿ÷åéêè*/
void change_pos(map* coor, short part, int size)
{
	con_draw_lock();
	int x = (con_width_new() - size * 5) / 2 + 6 * coor->x, cur_x = x;
	int y = (con_height_new() - size * 3) / 2 + 3 * coor->y;

	if (part == 0)
	{
		con_set_color(FONT, CON_CLR_BLACK);
	}
	else
	{
		con_set_color(CON_CLR_BLUE, CON_CLR_BLACK);
	}

	for (int k = 0; k < 5; k++, x++)
	{
		gotoxy(x, y);
		if (k == 0 || k == 4)
			fprintf(stdout, "+");
		else
			fprintf(stdout, "-");
	}
	y++;
	gotoxy(cur_x, y);
	fprintf(stdout, "|");
	gotoxy(x - 1, y);
	fprintf(stdout, "|");
	y++;
	for (int k = 0; k < 5; k++, cur_x++)
	{
		gotoxy(cur_x, y);
		if (k == 0 || k == 4)
			fprintf(stdout, "+");
		else
			fprintf(stdout, "-");
	}

	con_draw_release();
}

/*Âûáîð ÿ÷ååê*/
void select_(map* coor, int size, cell** field, int var)
{
	int x = (con_width_new() - size * 5) / 2 + 6 * coor->x + 2;
	int y = (con_height_new() - size * 3) / 2 + 3 * coor->y + 1;

	if (!var)
		con_set_color(CON_CLR_RED, CON_CLR_BLACK);

	gotoxy(x, y);
	fprintf(stdout, "%c", field[coor->y + 1][coor->x + 1].letter);
	con_set_color(FONT, CON_CLR_BLACK);
}

/*Ïðîâåðêà ÿâëÿåòñÿ ÿ÷åéêà ïóñòîé èëè ñìåæíîé*/
int check(map* selected, map* coor, cell** field, int idx)
{
	if (field[coor->y + 1][coor->x + 1].letter == ' ')
		return false_;

	if (idx == -1)
		return true_;

	for (int i = 0; i < idx; i++)
	{
		if (selected[i].x == coor->x && selected[i].y == coor->y)
			return 0;
	}

	int first = selected[idx].x, second = selected[idx].y;

	/*Ïîâòîðíîå íàæàòèå ïî ïîëþ*/
	if (coor->x == first && coor->y == second) return 2;

	if (coor->x == first && coor->y == second + 1) return 1;
	if (coor->x == first && coor->y == second - 1) return 1;
	if (coor->x == first + 1 && coor->y == second) return 1;
	if (coor->x == first - 1 && coor->y == second) return 1;

	return 0;
}

/*Ïðîâåðêà ñëîâà*/
int test(map* selected, int* idx, cell** field, int size, map* new_coor)
{
	con_draw_lock();

	if (size == 5 || size == 7)
	{
		gotoxy((con_width_new() - strlen("Ïðîâåðèòü ñëîâî")) / 2 + 2, 1);
		con_set_color(FONT, CON_CLR_GRAY);
		fprintf(stdout, "Ïðîâåðèòü ñëîâî");
		con_set_color(FONT, CON_CLR_BLACK);
	}

	if (size == 10)
	{
		gotoxy(con_width_new() - strlen("Ïðîâåðèòü ñëîâî") - 6, 0);
		con_set_color(FONT, CON_CLR_GRAY);
		fprintf(stdout, "Ïðîâåðèòü ñëîâî");
		con_set_color(FONT, CON_CLR_BLACK);
	}

	con_draw_release();

	short go = true_;
	while (go)
	{

		while (!key_is_pressed())
		{
			short code = key_pressed_code();
			if (code == KEY_DOWN && size != 10)
			{
				gotoxy((con_width_new() - strlen("Ïðîâåðèòü ñëîâî")) / 2 + 2, 1);
				fprintf(stdout, "Ïðîâåðèòü ñëîâî");
				return;
			}
			if (code == KEY_LEFT && size == 10)
			{
				gotoxy(con_width_new() - strlen("Ïðîâåðèòü ñëîâî") - 6, 0);
				fprintf(stdout, "Ïðîâåðèòü ñëîâî");
				return;
			}
			if (code == KEY_ENTER)
			{
				go = false_;
				break;
			}
		}
		while (key_is_pressed())
			key_pressed_code();
	}


	/*ñëîâî íå íàéäåíî*/
	if (*idx == 0)
	{
		if (size != 10)
		{
			gotoxy((con_width_new() - strlen("Ïðîâåðèòü ñëîâî")) / 2 + 2, 1);
			fprintf(stdout, "Ïðîâåðèòü ñëîâî");
			return;
		}
		if (size == 10)
		{
			gotoxy(con_width_new() - strlen("Ïðîâåðèòü ñëîâî") - 6, 0);
			fprintf(stdout, "Ïðîâåðèòü ñëîâî");
			return;
		}
		return;
	}

	/*Çàïîëíåíèå òåêóùåãî ñëîâà*/
	char* word = (char*)malloc(*idx + 1);
	if (word != NULL)
		memset(word, 0, *idx + 1);
	else
		exit(1);
	for (int i = 0; i < *idx; i++)
	{
		word[i] = field[selected[i].y + 1][selected[i].x + 1].letter;
	}
	word[*idx] = '\0';

	int yap = 0;
	/*Èñïîëüçîâàëàñü ëè ïîñòàâëåííàÿ áóêâà*/
	for (int i = 0; i < *idx; i++)
	{
		if (selected[i].x == new_coor->x && selected[i].y == new_coor->y)
		{
			yap++;
			break;
		}
	}

	if (yap)
	{
		if (main_search(root, word))
		{
			if (size != 10)
			{
				gotoxy((con_width_new() - strlen("Ñëîâî äîáàâëåíî!")) / 2 + 2, 2);
				if (search_in_list(word))
				{
					fprintf(stdout, "Ñëîâî äîáàâëåíî!");
					add_to_list(word);
					if (!comp)
						point_1 += strlen(word);
					else
					{
						if (move)
							point_2 += strlen(word);
						else
							point_1 += strlen(word);
					}
					gotoxy(4, 1);
					if (point_1 / 10 < 1)
						fprintf(stdout, "    %d    |     %d     ", point_1, point_2);
					else if (point_1 / 100 < 1)
						fprintf(stdout, "    %d   |     %d     ", point_1, point_2);
					else if (point_1 / 1000 < 1)
						fprintf(stdout, "    %d  |     %d     ", point_1, point_2);
					/*Ñhange_cells + äîáàâëåíèå î÷êîâ è òä è òï*/
					delete_red(selected, *idx, size, field);
					last_stand = 0;
					*idx = 0;
					new_coor->x = -1;
					new_coor->y = -1;
					if (word_y < 25)
					{
						gotoxy(11, word_y++);
						fprintf(stdout, word);
					}
					else
					{
						clean_words();
					}
					/*Õîä áîòà*/
					if (!comp)
					{
						if (mrOvechkin_move(field, size) == 1)
							return 1;
					}
					else
						move = move == 0 ? 1 : 0;
				}
			}
			else
			{
				gotoxy(con_width_new() - strlen("Ñëîâî äîáàâëåíî!") - 6, 1);
				if (search_in_list(word))
				{
					fprintf(stdout, "Ñëîâî äîáàâëåíî!");
					add_to_list(word);
					if (!comp)
						point_1 += strlen(word);
					else
					{
						if (move)
							point_2 += strlen(word);
						else
							point_1 += strlen(word);
					}
					gotoxy(4, 1);
					if (point_1 / 10 < 1)
						fprintf(stdout, "    %d    |     %d     ", point_1, point_2);
					else if (point_1 / 100 < 1)
						fprintf(stdout, "    %d   |     %d     ", point_1, point_2);
					else if (point_1 / 1000 < 1)
						fprintf(stdout, "    %d  |     %d     ", point_1, point_2);
					/*Ñhange_cells + äîáàâëåíèå î÷êîâ è òä è òï*/
					delete_red(selected, *idx, size, field);
					new_coor->x = -1;
					new_coor->y = -1;
					if (word_y < 25)
					{
						gotoxy(11, word_y++);
						fprintf(stdout, word);
					}
					else
					{
						clean_words();
					}
					last_stand = 0;
					*idx = 0;
					/*Õîä áîòà*/
					if (!comp)
					{
						if (mrOvechkin_move(field, size) == 1)
							return 1;
					}
					else
						move = move == 0 ? 1 : 0;
				}
			}
		}
		else
		{
			if (size != 10)
			{
				gotoxy((con_width_new() - strlen("Ñëîâà íåò â ñëîâàðå")) / 2 + 2, 2);
				fprintf(stdout, "Ñëîâà íåò â ñëîâàðå");
			}
			else
			{
				gotoxy(con_width_new() - strlen("Ñëîâà íåò â ñëîâàðå") - 4, 1);
				fprintf(stdout, "Ñëîâà íåò â ñëîâàðå");
			}
		}
	}

	pause(2000);

	if (size != 10)
	{
		gotoxy((con_width_new() - strlen("Ïðîâåðèòü ñëîâî")) / 2 + 2, 1);
		fprintf(stdout, "Ïðîâåðèòü ñëîâî");
		gotoxy((con_width_new() - strlen("Ñëîâà íåò â ñëîâàðå")) / 2 + 2, 2);
		fprintf(stdout, "                   ");
	}
	if (size == 10)
	{
		gotoxy(con_width_new() - strlen("Ïðîâåðèòü ñëîâî") - 6, 0);
		fprintf(stdout, "Ïðîâåðèòü ñëîâî");
		gotoxy(con_width_new() - strlen("Ñëîâà íåò â ñëîâàðå") - 4, 1);
		fprintf(stdout, "                   ");
	}

	return 0;
}

/*Ìèñòåð Îâå÷êèí âûìîòàë èãðîêà*/
int give_up()
{
	con_draw_lock();

	con_set_color(FONT, CON_CLR_GRAY);
	gotoxy(2, con_height_new() - 2);
	fprintf(stdout, "Ñäàòüñÿ");

	con_draw_release();
	con_set_color(FONT, CON_CLR_BLACK);

	while (true_)
	{
		while (!key_is_pressed())
		{
			short code = key_pressed_code();
			if (code == KEY_ENTER)
			{
				con_set_color(FONT, CON_CLR_BLACK);
				clrscr();
				gotoxy((con_width_new() - strlen("Â ñëåäóþùèé ðàç ïîâåçåò!")) / 2, con_height_new() / 2);
				fprintf(stdout, "Â ñëåäóþùèé ðàç ïîâåçåò!");
				pause(3000);
				return 1;
			}
			if (code == KEY_RIGHT)
			{
				con_set_color(FONT, CON_CLR_BLACK);
				gotoxy(2, con_height_new() - 2);
				fprintf(stdout, "Ñäàòüñÿ");
				return 0;
			}
		}
		while (key_is_pressed())
			key_pressed_code();
	}
}

/*Ðàçìåð çàãðóçî÷íîãî ôàéëà*/
int file_size()
{
	saved = fopen(DOWNLOAD, "r");
	fseek(saved, 0, SEEK_END);
	int size = ftell(saved);
	fseek(saved, 0, SEEK_SET);
	return size;
}

/*Âûáîð ñîõðàíÿòü ëè ïðîãðåññ*/
void save_your_tears(int size, cell** field)
{
	clrscr();
	con_draw_lock();

	saved = fopen(DOWNLOAD, "w");

	int x = con_width_new(), y = con_height_new() / 2 - 7, active = 0;

	gotoxy((x - strlen("Ñîõðàíèòü èãðó?")) / 2, y);
	fprintf(stdout, "Ñîõðàíèòü èãðó?");
	con_set_color(FONT, CON_CLR_GRAY);
	gotoxy((x - strlen("Äà Íåò")) / 2 - 1, y + 2);
	fprintf(stdout, "Äà");
	con_set_color(FONT, CON_CLR_BLACK);
	gotoxy((x - strlen("Äà Íåò")) / 2 + 4, y + 2);
	fprintf(stdout, "Íåò");

	con_draw_release();

	while (true_)
	{
		while (!key_is_pressed())
		{
			short code = key_pressed_code();
			if (code == KEY_RIGHT)
			{
				if (active < 1)
				{
					active++;
					con_set_color(FONT, CON_CLR_BLACK);
					gotoxy((x - strlen("Äà Íåò")) / 2 - 1, y + 2);
					fprintf(stdout, "Äà");
					con_set_color(FONT, CON_CLR_GRAY);
					gotoxy((x - strlen("Äà Íåò")) / 2 + 4, y + 2);
					fprintf(stdout, "Íåò");
					con_set_color(FONT, CON_CLR_BLACK);
				}
			}
			if (code == KEY_LEFT)
			{
				if (active > 0)
				{
					active--;
					con_set_color(FONT, CON_CLR_GRAY);
					gotoxy((x - strlen("Äà Íåò")) / 2 - 1, y + 2);
					fprintf(stdout, "Äà");
					con_set_color(FONT, CON_CLR_BLACK);
					gotoxy((x - strlen("Äà Íåò")) / 2 + 4, y + 2);
					fprintf(stdout, "Íåò");
				}
			}
			if (code == KEY_ENTER)
			{
				if (active == 1)
				{
					fclose(saved);
					return;
				}
				/*Ðàçìåð ïîëÿ + óðîâåíü ñëîæíîñòè*/
				fprintf(saved, "%d %d %d %d %d\n", size, lvl, comp, word_y, move);
				/*Î÷êè èãðîêîâ*/
				fprintf(saved, "%d %d\n", point_1, point_2);
				/*Çàïîëíåíèå ïîëÿ*/
				for (int i = 1; i < size + 1; i++)
				{
					for (int j = 1; j < size + 1; j++)
					{
						fprintf(saved, "%c", field[i][j].letter);
					}
					fprintf(saved, "\n");
				}
				/*Ñîõðàíåíèå èñïîëüçîâàííûõ ñëîâ*/
				added* current = list;
				while (current != NULL)
				{
					fprintf(saved, "%s\n", current->word);
					current = current->next;
				}

				fclose(saved);

				return;
			}
		}
		while (key_is_pressed())
			key_pressed_code();
	}

}

/*Íà÷àëî èãðû*/
int start_game(int size, cell** field, int g)
{
	clrscr();
	con_draw_lock();
	int x = (con_width_new() - size * 5) / 2, y = (con_height_new() - size * 3) / 2;
	int prev = 0, idx_1 = 0;
	map selected[150];
	memset(selected, 0, 150 * sizeof(map));

	for (int i = 0; i < size; i++)
	{
		int cur_x = x, cur_x_1 = x;
		for (int j = 0; j < size; j++)
		{
			if (i == 0 && j == 0)
				con_set_color(CON_CLR_BLUE, CON_CLR_BLACK);
			else
				con_set_color(FONT, CON_CLR_BLACK);
			for (int k = 0; k < 5; k++, cur_x++)
			{
				gotoxy(cur_x, y);
				if (k == 0 || k == 4)
					fprintf(stdout, "+");
				else
					fprintf(stdout, "-");
			}
			y++;
			gotoxy(cur_x_1, y);
			fprintf(stdout, "|");
			gotoxy(cur_x - 1, y);
			fprintf(stdout, "|");
			y++;
			for (int k = 0; k < 5; k++, cur_x_1++)
			{
				gotoxy(cur_x_1, y);
				if (k == 0 || k == 4)
					fprintf(stdout, "+");
				else
					fprintf(stdout, "-");
			}
			y -= 2;
			cur_x++;
			cur_x_1++;
		}
		y += 3;
	}

	if (g == 0)
	{
		char slovoSPB[15];
		memset(slovoSPB, 0, 10);
		if (size == 5)
		{
			strcpy(slovoSPB, "áàëäà");
			add_to_list("áàëäà");
		}
		if (size == 7)
		{
			strcpy(slovoSPB, "òðàêòîð");
			add_to_list("òðàêòîð");
		}
		if (size == 10)
		{
			strcpy(slovoSPB, "ìàðìåëàäêà");
			add_to_list("ìàðìåëàäêà");
		}
		int idx = 0;
		y = (con_height_new() - size * 3) / 2;
		if (size == 5 || size == 7)
		{
			for (int i = 0; i < size; i++)
			{
				gotoxy(x + 2 + 6 * i, y + 3 * (size / 2) + 1);
				fprintf(stdout, "%c", slovoSPB[idx++]);
			}
		}
		else
		{
			for (int i = 0; i < size; i++)
			{
				gotoxy(x + 2 + 6 * i, y + 3 * i + 1);
				fprintf(stdout, "%c", slovoSPB[idx++]);
			}
		}
	}
	else
	{
		y = (con_height_new() - size * 3) / 2 + 1;
		for (int i = 1; i < size + 1; i++, y += 3)
		{
			for (int j = 1; j < size + 1; j++)
			{
				gotoxy(x + 2 + 6 * (j - 1), y);
				fprintf(stdout, "%c", field[i][j].letter);
			}
		}
		added* current_ = list->next;
		x = 11, y = 3;
		while (current_ != NULL)
		{
			gotoxy(x, y);
			y++;
			fprintf(stdout, "%s", current_->word);
			current_ = current_->next;
		}
	}

	gotoxy(con_width_new() - strlen("Äëÿ âûõîäà íàæìèòå ESC") - 2, con_height_new() - 2);
	fprintf(stdout, "Äëÿ âûõîäà íàæìèòå ESC");
	if (size == 10)
	{
		gotoxy(con_width_new() - strlen("Ïðîâåðèòü ñëîâî") - 6, 0);
		fprintf(stdout, "Ïðîâåðèòü ñëîâî");
	}
	else
	{
		gotoxy((con_width_new() - strlen("Ïðîâåðèòü ñëîâî")) / 2 + 2, 1);
		fprintf(stdout, "Ïðîâåðèòü ñëîâî");
	}

	if (!comp)
	{
		gotoxy(4, 0);
		fprintf(stdout, "  Èãðîê  | Êîìïüþòåð ");
		gotoxy(4, 1);
		if (point_1 / 10 < 1)
			fprintf(stdout, "    %d    |     %d     ", point_1, point_2);
		else if (point_1 / 100 < 1)
			fprintf(stdout, "    %d   |     %d     ", point_1, point_2);
		else if (point_1 / 1000 < 1)
			fprintf(stdout, "    %d  |     %d     ", point_1, point_2);
	}
	else
	{
		gotoxy(4, 0);
		fprintf(stdout, "  Èãðîê  |  Èãðîê  ");
		gotoxy(4, 1);
		if (point_1 / 10 < 1)
			fprintf(stdout, "    %d    |     %d     ", point_1, point_2);
		else if (point_1 / 100 < 1)
			fprintf(stdout, "    %d   |     %d     ", point_1, point_2);
		else if (point_1 / 1000 < 1)
			fprintf(stdout, "    %d  |     %d     ", point_1, point_2);
	}

	gotoxy(2, con_height_new() - 2);
	fprintf(stdout, "Ñäàòüñÿ");

	con_draw_release();

	map* coordinates = (map*)malloc(sizeof(map));
	map* new_coor = (map*)malloc(sizeof(map));
	if (coordinates != NULL && new_coor != NULL)
	{
		memset(coordinates, 0, sizeof(map));
		memset(new_coor, 0, sizeof(map));
	}
	else
		exit(1);

	while (true_)
	{
		short code = 0;
		while (!key_is_pressed())
		{
			code = key_pressed_code();
			if ((code > 223 && code < 256 || code == 480) && last_stand == 0)
			{
				if (field[coordinates->y + 1][coordinates->x + 1].letter == ' ')
				{
					gotoxy((con_width_new() - size * 5) / 2 + 6 * coordinates->x + 2,
						(con_height_new() - size * 3) / 2 + 3 * coordinates->y + 1);
					fprintf(stdout, "%c", code);
					last_stand++;
					field[coordinates->y + 1][coordinates->x + 1].letter = code;
					new_coor->x = coordinates->x;
					new_coor->y = coordinates->y;
				}
			}
			if (code == KEY_ESC)
			{
				con_set_color(FONT, CON_CLR_BLACK);
				/*Ñîõðàíÿåì?*/
				save_your_tears(size, field);

				point_1 = 0, point_2 = 0, move = 0, word_y = 3;
				list->next = NULL;
				list->word[0] = '\0';
				free(field);
				return 1;
			}
			if (code == KEY_UP)
			{
				if (coordinates->y == 0 && (size == 5 || size == 7))
				{
					if (test(selected, &idx_1, field, size, new_coor) == 1)
						return 1;
				}

				if (coordinates->y > 0)
				{
					if (prev == 0)
						change_pos(coordinates, 0, size);
					else
						prev = 1;
					coordinates->y--;
					change_pos(coordinates, 1, size);
					con_set_color(FONT, CON_CLR_BLACK);
				}
			}
			if (code == KEY_RIGHT)
			{

				if (coordinates->x == size - 1 && (size == 10))
				{
					if (test(selected, &idx_1, field, size, new_coor) == 1)
						return 1;
				}

				if (coordinates->x < size - 1)
				{
					if (prev == 0)
						change_pos(coordinates, 0, size);
					else
						prev = 1;
					coordinates->x++;
					change_pos(coordinates, 1, size);
					con_set_color(FONT, CON_CLR_BLACK);
				}
			}
			if (code == KEY_DOWN)
			{
				if (coordinates->y < size - 1)
				{
					if (prev == 0)
						change_pos(coordinates, 0, size);
					else
						prev = 0;
					coordinates->y++;
					change_pos(coordinates, 1, size);
					con_set_color(FONT, CON_CLR_BLACK);
				}
			}
			if (code == KEY_LEFT)
			{
				if (coordinates->x == 0)
				{
					if (give_up())
					{
						point_1 = 0, point_2 = 0, move = 0, word_y = 3;
						list->next = NULL;
						list->word[0] = '\0';
						free(field);
						return 1;
					}
				}

				if (coordinates->x > 0)
				{
					if (prev == 0)
						change_pos(coordinates, 0, size);
					else prev = 0;
					coordinates->x--;
					change_pos(coordinates, 1, size);
					con_set_color(FONT, CON_CLR_BLACK);
				}
			}
			if (code == KEY_ENTER)
			{
				/*Ïðîâåðêà ñìåæíàÿ ëè êëåòêà*/
				int result = check(selected, coordinates, field, idx_1 - 1);
				if (!result)
					/*Êëåòêà íå ñìåæíàÿ*/
					break;

				if (result == 2)
				{
					select_(coordinates, size, field, 1);
					idx_1--;
					break;
				}

				selected[idx_1].x = coordinates->x;
				selected[idx_1++].y = coordinates->y;

				select_(coordinates, size, field, 0);
			}
			if (code == KEY_BACK)
			{
				if (!last_stand)
					break;
				if (coordinates->x == new_coor->x && coordinates->y == new_coor->y)
				{
					delete_red(selected, idx_1, size, field);
					idx_1 = 0;
					field[coordinates->y + 1][coordinates->x + 1].letter = ' ';
					gotoxy((con_width_new() - size * 5) / 2 + 6 * coordinates->x + 2,
						(con_height_new() - size * 3) / 2 + 3 * coordinates->y + 1);
					fprintf(stdout, " ");
					last_stand--;
				}
			}
		}

		while (key_is_pressed())
			key_pressed_code();

		pause(40);
	}
}

/*Âûáîð ïîëüçîâàòåëåì ïàðàìåòðîâ èãðû*/
int game_parametres()
{
	clrscr();
	con_draw_lock();

	short active = 0, size = 0;
	char* vars[] = { "Ëåãêî", "Ñðåäíå", "Ñëîæíî" ,"5","7","10",
		"Äâà èãðîêà", "Ïðîòèâ êîìïüþòåðà" };

	int x = con_width_new(), y = con_height_new() / 2 - 7;
	gotoxy(x - strlen("Äëÿ âûõîäà íàæìèòå ESC") - 2, con_height_new() - 2);
	fprintf(stdout, "Äëÿ âûõîäà íàæìèòå ESC");
	gotoxy(2, con_height_new() - 2);
	fprintf(stdout, "*Äëÿ âûáîðà èñïîëüçóéòå ñòðåëêè âïðàâî èëè âëåâî");
	gotoxy((x - strlen("Âûáåðèòå óðîâåíü ñëîæíîñòè")) / 2, y);
	fprintf(stdout, "Âûáåðèòå óðîâåíü ñëîæíîñòè");
	gotoxy((x - strlen(vars[0])) / 2, y + 2);
	con_set_color(FONT, CON_CLR_GRAY);
	fprintf(stdout, "%s", vars[0]);
	con_draw_release();

	/*Ïåðåìåííûå äëÿ èãðû ñ áîòîì*/
	int field_size = 0;
	cell** field = NULL;

	short go = true_;
	while (go)
	{
		short code = 0;
		while (!key_is_pressed())
		{
			code = key_pressed_code();
			if (code == KEY_ESC)
			{
				con_set_color(FONT, CON_CLR_BLACK);
				return 0;
			}
			if (code == KEY_RIGHT)
			{
				if (active < 2)
				{
					active++;
					change_lvl(active, vars, 1, 0);
				}
			}
			if (code == KEY_LEFT)
			{
				if (active > 0)
				{
					active--;
					change_lvl(active, vars, 0, 0);
				}
			}
			if (code == KEY_ENTER)
			{
				con_set_color(FONT, CON_CLR_BLACK);
				/*Óðîâåíü ñëîæíîñòè*/
				lvl = active + 1;
				go = false_;
				break;
			}
		}

		while (key_is_pressed())
			key_pressed_code();

		pause(40);
	}

	con_draw_lock();

	gotoxy((x - strlen("Âûáåðèòå ðàçìåð äèàãîíàëè")) / 2, y + 4);
	fprintf(stdout, "Âûáåðèòå ðàçìåð äèàãîíàëè");
	gotoxy((x - strlen(vars[3])) / 2, y + 6);
	con_set_color(FONT, CON_CLR_GRAY);
	fprintf(stdout, "%s", vars[3]);
	active = 3;
	con_draw_release();

	go = true_;
	while (go)
	{
		short code = 0;
		while (!key_is_pressed())
		{
			code = key_pressed_code();
			if (code == KEY_ESC)
			{
				con_set_color(FONT, CON_CLR_BLACK);
				return;
			}
			if (code == KEY_RIGHT)
			{
				if (active < 5)
				{
					active++;
					change_lvl(active, vars, 1, 1);
				}
			}
			if (code == KEY_LEFT)
			{
				if (active > 3)
				{
					active--;
					change_lvl(active, vars, 0, 1);
				}
			}
			if (code == KEY_ENTER)
			{
				con_set_color(FONT, CON_CLR_BLACK);
				if (active == 3)
					size = 5;
				if (active == 4)
					size = 7;
				if (active == 5)
					size = 10;

				field_size = size;
				field = create_FIELD(&field_size);
				go = false_;
				break;
			}
		}

		while (key_is_pressed())
			key_pressed_code();

		pause(40);
	}

	con_draw_lock();

	gotoxy((x - strlen("Âûáåðèòå ðåæèì èãðû")) / 2, y + 8);
	fprintf(stdout, "Âûáåðèòå ðåæèì èãðû");
	gotoxy((x - strlen(vars[6])) / 2, y + 10);
	con_set_color(FONT, CON_CLR_GRAY);
	fprintf(stdout, "%s", vars[6]);
	active = 6;
	con_draw_release();

	go = true_;
	while (go)
	{
		short code = 0;
		while (!key_is_pressed())
		{
			code = key_pressed_code();
			if (code == KEY_ESC)
			{
				con_set_color(FONT, CON_CLR_BLACK);
				return;
			}
			if (code == KEY_RIGHT)
			{
				if (active < 7)
				{
					active++;
					change_lvl(active, vars, 1, 2);
				}
			}
			if (code == KEY_LEFT)
			{
				if (active > 6)
				{
					active--;
					change_lvl(active, vars, 0, 2);
				}
			}
			if (code == KEY_ENTER)
			{
				if (active == 6)
					comp = 1;
				else
					comp = 0;
				con_set_color(FONT, CON_CLR_BLACK);
				go = false_;
				break;
			}
		}

		while (key_is_pressed())
			key_pressed_code();

		pause(40);
	}

	con_set_color(FONT, CON_CLR_BLACK);
	if (start_game(size, field, 0))
		return 1;
}

/*Çàãðóùêà ñîõðàíåííîé èãðû*/
void dwnld()
{
	if (!file_size())
	{
		clrscr();
		gotoxy((con_width_new() - strlen("Íåò ñîõðàíåííûõ èãð!")) / 2, con_height_new() / 2 - 7);
		fprintf(stdout, "Íåò ñîõðàíåííûõ èãð!");
		pause(3000);
		fclose(saved);
		return;
	}

	char num[30];
	if (num != NULL)
		memset(num, 0, 30);
	else
		exit(1);
	/*Ðàçìåð ïîëÿ è óðîâåíü ñëîæíîñòè*/
	fgets(num, 30, saved);
	int field_size = 0, stop = 0;
	for (int i = 0; i < 10; i++)
	{
		if (num[i] == ' ')
		{
			stop = i + 1;
			break;
		}
		field_size = field_size * pow(10, i) + num[i] - '0';
	}
	lvl = num[stop] - '0';

	comp = num[stop + 2] - '0';

	word_y = num[stop + 4] - '0';

	move = num[stop + 6] - '0';

	num[0] = '\0';
	fgets(num, 30, saved);

	for (int i = 0; i < 10; i++)
	{
		if (num[i] == ' ')
		{
			stop = i + 1;
			break;
		}
		point_1 = point_1 * pow(10, i) + num[i] - '0';
	}

	int new_i = 0;
	while (num[stop] != '\n')
	{
		point_2 = point_2 * pow(10, new_i++) + num[stop++] - '0';
	}

	num[0] = '\0';

	cell** field = create_FIELD(&field_size);

	for (int i = 1; i <= field_size; i++)
	{
		for (int j = 1; j <= field_size; j++)
		{
			if (fscanf(saved, "%c", &field[i][j].letter) != 1)
				exit(1);
		}
		/*Èãíîðèðîâàíèå \n*/
		fseek(saved, 2, SEEK_CUR);
	}

	while (fgets(num, 30, saved) != NULL)
	{
		if (num[strlen(num) - 1] == '\n')
			num[strlen(num) - 1] = '\0';
		add_to_list(num);
	}

	fclose(saved);

	start_game(field_size, field, 1);
}

/*Âûáîð òèïà èãðû*/
void game_move()
{
	char* vars[] = { "Íîâàÿ èãðà", "Çàãðóçèòü èãðó" };
	short active_button = 0;
	menu_game(active_button, vars);

	while (true_)
	{
		short code = 0;
		while (!key_is_pressed())
		{
			code = key_pressed_code();
			/*Âûõîä*/
			if (code == KEY_ESC)
			{
				return;
			}
			if (code == KEY_UP)
			{
				if (active_button > 0)
				{
					changer(active_button, vars, 0);
					active_button--;
					changer(active_button, vars, 1);
					break;
				}
			}
			if (code == KEY_DOWN)
			{
				if (active_button < 1)
				{
					changer(active_button, vars, 0);
					active_button++;
					changer(active_button, vars, 1);
					break;
				}
			}
			/*Âûáðàíà êàòåãîðèÿ*/
			if (code == KEY_ENTER)
			{
				/*Âûáîð ðàçäåëà ìåíþ*/
				switch (active_button)
				{
				case(0):
					if (game_parametres() == 1)
						return;
					menu_game(active_button, vars);
					break;
				case(1):
					dwnld();
					menu_game(active_button, vars);
					break;
				}
			}
		}

		while (key_is_pressed())
			key_pressed_code();

		pause(40);
	}
}

/*Ñïðàâêà <-> èíñòðóêöèÿ*/
void reference()
{
	clrscr();
	con_draw_lock();

	int x = con_width_new(), y = con_height_new() / 2 - 14;
	gotoxy(4, y); //(x - strlen("Óïðàâëåíèå")) / 2
	y += 2;
	con_set_color(CON_CLR_BLUE, CON_CLR_BLACK);
	fprintf(stdout, "Óïðàâëåíèå");
	con_set_color(FONT, CON_CLR_BLACK);
	gotoxy(4, y);
	y += 2;
	fprintf(stdout, "[1] Ïåðåìåùåíèå ïî ïîëþ ââåðõ - [KEY_UP].");
	gotoxy(4, y);
	y += 2;
	fprintf(stdout, "[2] Ïåðåìåùåíèå ïî ïîëþ âïðàâî - [KEY_RIGHT].");
	gotoxy(4, y);
	y += 2;
	fprintf(stdout, "[3] Ïåðåìåùåíèå ïî ïîëþ âíèç - [KEY_DOWN].");
	gotoxy(4, y);
	y += 2;
	fprintf(stdout, "[4] Ïåðåìåùåíèå ïî ïîëþ âëåâî - [KEY_LEFT].");
	gotoxy(4, y);
	y += 2;
	fprintf(stdout, "[5] Âûäåëåíèå áóêâû - [KEY_ENTER].");
	gotoxy(4, y);
	y += 2;
	fprintf(stdout, "[6] Ñíÿòèå âûäåëåíèÿ - [KEY_ENTER]+[KEY_ENTER].");
	gotoxy(4, y);
	y += 2;
	fprintf(stdout, "[7] Óáðàòü ïîñòàâëåííóþ áóêâó - [KEY_BACK].");
	gotoxy(4, y);
	y += 2;
	fprintf(stdout, "[8] Ïîñòàâèòü áóêâó - ëþáàÿ áóêâà íà êèðèëëèöå.");


	gotoxy(x - strlen("Äëÿ âûõîäà íàæìèòå ESC") - 2, con_height_new() - 2);
	fprintf(stdout, "Äëÿ âûõîäà íàæìèòå ESC");

	con_draw_release();

	while (true_)
	{
		while (!key_is_pressed())
		{
			short code = key_pressed_code();
			if (code == KEY_ESC)
				return;
		}
		while (key_is_pressed())
			key_pressed_code();
	}
}

/*Äîáàâëåíèå ñëîâ â ñëîâàðü + ðàçúÿñíåíèÿ*/
void settings(FILE* dict)
{
	clrscr();
	con_draw_lock();

	int x = con_width_new(), y = con_height_new() / 2 - 7;
	con_set_color(FONT, CON_CLR_GRAY);
	gotoxy((x - strlen("Äîáàâèòü íîâîå ñëîâî â ñëîâàðü")) / 2, y);
	fprintf(stdout, "Äîáàâèòü íîâîå ñëîâî â ñëîâàðü");
	con_set_color(FONT, CON_CLR_BLACK);
	gotoxy((x - strlen("Îñíîâíûå èãðîâûå íàñòðîéêè çàäàþòñÿ â ðàçäåëå \"Íîâàÿ èãðà\"")) / 2, y + 5);
	fprintf(stdout, "Îñíîâíûå èãðîâûå íàñòðîéêè çàäàþòñÿ â ðàçäåëå \"Íîâàÿ èãðà\"");
	gotoxy(x - strlen("Äëÿ âûõîäà íàæìèòå ESC") - 2, con_height_new() - 2);
	fprintf(stdout, "Äëÿ âûõîäà íàæìèòå ESC");

	con_draw_release();
	char new_word[30];
	if (new_word != NULL)
		memset(new_word, 0, 30);

	while (true_)
	{
		while (!key_is_pressed())
		{
			short code = key_pressed_code();
			if (code == KEY_ESC)
				return;
			if (code == KEY_ENTER)
			{
				show_cursor(1);
				gotoxy((x - strlen("Äîáàâèòü íîâîå ñëîâî â ñëîâàðü")) / 2, y + 2);
				if (fscanf(stdin, "%25s", new_word) != 1)
					exit(1);
				insert(new_word, root);
				fprintf(dict, "\n%s", new_word);
				new_word[0] = '\0';
				gotoxy((x - strlen("Äîáàâèòü íîâîå ñëîâî â ñëîâàðü")) / 2, y + 2);
				fprintf(stdout, "                                                                  ");
				gotoxy((x - strlen("Óñïåøíî!")) / 2, y + 2);
				fprintf(stdout, "Óñïåøíî!");
				pause(3000);
				gotoxy((x - strlen("Äîáàâèòü íîâîå ñëîâî â ñëîâàðü")) / 2, y + 2);
				fprintf(stdout, "                                                                  ");
				show_cursor(0);
			}
		}
		while (key_is_pressed())
			key_pressed_code();
	}
}

/*Âñòàâêà â äåðåâî ðåêîðäîâ*/
void ins_rec(int num, char* wrd, vsOv** rt)
{
	vsOv* node, * current = *rt, * prev = NULL;
	node = (vsOv*)malloc(sizeof(vsOv));
	if (node == NULL)
		exit(1);
	memset(node, 0, sizeof(vsOv));
	node->points = num;
	strcpy(node->name, wrd);
	node->left = NULL;
	node->right = NULL;

	while (current != NULL)
	{
		prev = current;
		current = current->points < node->points ? current->right : current->left;
	}

	/*Âåðøèíà*/
	if (prev == NULL)
	{
		*rt = node;
		return;
	}
	else
	{
		current = node;
		if (prev->points > node->points)
			prev->left = node;
		else
			prev->right = node;
	}
	return;
}

/*Âûâîä äåðåâà*/
void pr_nodes(vsOv* rt, int x, int* y, int* nums)
{
	int done = 0;
	if (rt->right == NULL)
	{
		gotoxy(x, *y);
		fprintf(stdout, "[%d] %s - %d", *nums, rt->name, rt->points);
		*y = *y + 2;
		*nums = *nums + 1;
		done++;
	}
	else
		pr_nodes(rt->right, 4, y, nums);

	if (rt->left != NULL)
		pr_nodes(rt->left, 4, y, nums);
	else
	{
		if (done)
			return;
		gotoxy(x, *y);
		fprintf(stdout, "[%d] %s - %d", *nums, rt->name, rt->points);
		*y = *y + 2;
		*nums = *nums + 1;
	}
}

/*Ðåêóðñèâíûé âûâîä ðåêîðäîâ â êîíñîëü*/
void show_r(vsOv* rt)
{
	clrscr();
	con_draw_lock();
	int x = con_width_new(), y = 2;
	int nums = 1;


	if (rt == NULL)
	{
		gotoxy((x - strlen("Òàáëèöà ïóñòà!")) / 2, y);
		fprintf(stdout, "Òàáëèöà ïóñòà!");
		con_draw_release();
		pause(4000);
		return;
	}

	gotoxy(4, y);
	con_set_color(CON_CLR_BLUE, CON_CLR_BLACK);
	fprintf(stdout, "Òàáëèöà ðåêîðäîâ");
	con_set_color(FONT, CON_CLR_BLACK);
	y += 2;
	pr_nodes(rt, 4, &y, &nums);

	gotoxy(x - strlen("Äëÿ âûõîäà íàæìèòå ESC") - 2, con_height_new() - 2);
	fprintf(stdout, "Äëÿ âûõîäà íàæìèòå ESC");

	con_draw_release();

	while (true_)
	{
		while (!key_is_pressed())
		{
			short code = key_pressed_code();
			if (code == KEY_ESC)
				return;
		}
		while (key_is_pressed())
			key_pressed_code();
	}
}

/*Óäàëåíèå äåðåâà*/
void clr_tr(vsOv* rt)
{
	if (rt == NULL)
		return;
	if (rt->left == NULL && rt->right == NULL)
	{
		free(rt);
		return;
	}
	clr_tr(rt->left);
	clr_tr(rt->right);
}

/*Òàáëèöà ðåêîðäîâ*/
void rcrds_show()
{
	records = fopen(REC, "r");
	vsOv* rc_root = NULL;
	char wrd[30], final_[30];
	if (wrd == NULL || final_ == NULL)
		exit(1);
	memset(wrd, 0, 30);
	memset(final_, 0, 30);

	int num = 0;
	while (fgets(wrd, 30, records) != NULL && wrd[0] != '\n')
	{
		int idx = 0, idx_1 = 0;
		while (wrd[idx] != ' ')
		{
			num = num * 10 + (wrd[idx] - '0');
			idx++;
		}
		idx++;
		while (wrd[idx] != '\0' && wrd[idx] != '\n')
		{
			final_[idx_1] = wrd[idx];
			idx++;
			idx_1++;
		}
		final_[idx_1] = '\0';
		ins_rec(num, final_, &rc_root);
		num = 0;
	}

	show_r(rc_root);

	/*Î÷èñòêà äåðåâà + çàêðûòèå ôàéëà*/
	clr_tr(rc_root);
	fclose(records);
}

/*Ñìåùåíèå óêàçàòåëÿ ïî êëàâèøàì*/
void menu_move(FILE* dict)
{
	char* vars[] = { "Èãðàòü", "Ðåêîðäû", "Íàéñòðîéêè", "Ñïðàâêà", "Î ïðîãðàììå", "Âûõîä" };
	short active_button = 0;

	main_menu(vars, active_button);

	while (true_)
	{
		short code = 0;
		while (!key_is_pressed())
		{

			code = key_pressed_code();
			if (code == KEY_UP)
			{
				if (active_button > 0)
				{
					changer(active_button, vars, 0);
					active_button--;
					changer(active_button, vars, 1);
					break;
				}
			}
			if (code == KEY_DOWN)
			{
				if (active_button < 5)
				{
					changer(active_button, vars, 0);
					active_button++;
					changer(active_button, vars, 1);
					break;
				}
			}
			/*Âûáðàíà êàòåãîðèÿ*/
			if (code == KEY_ENTER)
			{
				/*Âûáîð ðàçäåëà ìåíþ*/
				switch (active_button)
				{
				case(0):
					game_move();
					main_menu(vars, active_button);
					break;
				case(1):
					rcrds_show();
					main_menu(vars, active_button);
					break;
				case(2):
					settings(dict);
					main_menu(vars, active_button);
					break;
				case(3):
					reference();
					main_menu(vars, active_button);
					break;
				case(4):
					about();
					main_menu(vars, active_button);
					break;
				case(5):
					fprintf(stdout, "\n\n\n\n\n");
					return;
				}
			}

		}

		while (key_is_pressed())
			key_pressed_code();

		pause(40);
	}

}

int main()
{
	/*Èíèöèëèçàöèÿ êîíñîëè + óñòàíîâêà êîäèðîâêè 1251*/
	con_init(80, 25);
	/*Ñêðûòèå êóðñîðà*/
	show_cursor(0);

	FILE* dict, * inv;

	/*Ñîçäàíèå ôàéëîâ è äåðåâüåâ*/
	create_FILES(&dict, &inv, &records);
	create_TRIES();

	/*Ñîçäàíèå äâóõ äåðåâüåâ*/
	dict_read(root, dict);
	dict_read(root_inv, inv);

	/*Ãðàôè÷åñêîå ìåíþ*/
	menu_move(dict);

	/*Çàêðûòèå ôàéëîâ-ñëîâàðåé*/
	fclose(dict);
	fclose(inv);

	return EXIT_SUCCESS;
}