#include <stdio.h>
#include <Windows.h>
#include <locale.h>

int sda() {

	setlocale(LC_ALL, "Russian");
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);
	for (int i = 224; i < 250; i++) {
		printf("%c -- %d\n", i, i);
	}

	return 0;
}