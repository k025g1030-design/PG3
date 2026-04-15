#include <iostream>
#include <Windows.h>

int main() {
    SetConsoleOutputCP(65001);
    char str[] = "こんにちは、世界一番の姫様\n";
	printf("%s", str);
    return 0;
}

