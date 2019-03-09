#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define CHANKSIZE 64
#define TRUE      1
#define FALSE     0

int read_raw_text(char*** raw_text);
int translate_text_to_low_case(char*** raw_text, int str_count, char*** cultivated_text);
void free_memory(char*** text, int str_count);

int main() {
	char** raw_text = NULL;
	char** cultivated_text = NULL;
	
	int str_count = 0;
	int cultivated_str_count = 0;

	str_count = read_raw_text(&raw_text);
	if (str_count < 0) {
		printf("[error]");
		return 0;
	}

	cultivated_str_count = translate_text_to_low_case(&raw_text, str_count, &cultivated_text);
	if (cultivated_str_count < 0) {
		printf("[error]");
		return 0;
	}

	for (int i = 0; i < cultivated_str_count; i++) {
		printf("%s", cultivated_text[i]);
	}

	free_memory(&raw_text, str_count);
	free_memory(&cultivated_text, cultivated_str_count);

	return 0;
}

int read_raw_text(char*** raw_text) {
	char chank[CHANKSIZE];

	int str_count = 0;
	size_t char_count = 0;
	int is_new_str = TRUE;
	*raw_text = (char**)malloc(sizeof(char*));
	if (*raw_text == NULL) {
		return -1;
	}

	while (fgets(chank, CHANKSIZE, stdin)) {
		char** reserv_ptr_1 = (char**)realloc((*raw_text), sizeof(char*) * (str_count + 1));
		if (reserv_ptr_1 == NULL) {
			free_memory(raw_text, str_count);
			return -1;
		} else {
			(*raw_text) = reserv_ptr_1;
		}

		if (is_new_str) {
			(*raw_text)[str_count] = (char*)malloc(sizeof(char));
			if ((*raw_text)[str_count] == NULL) {
				free_memory(raw_text, str_count);
				return -1;
			}
			is_new_str = FALSE;
		}

		char* reserv_ptr_2 = (char*)realloc((*raw_text)[str_count], sizeof(char) * (char_count + strlen(chank) + 1));
		if (reserv_ptr_2 == NULL) {
			free_memory(raw_text, str_count);
			return -1;
		} else {
			(*raw_text)[str_count] = reserv_ptr_2;
		}

		memcpy((*raw_text)[str_count] + char_count, chank, strlen(chank) + 1);
		char_count += strlen(chank);
		if ((strchr(chank, '\n')) || (feof(stdin))) {
			str_count++;
			char_count = 0;
			is_new_str = TRUE;
		}
	}

	return str_count;
}

int translate_text_to_low_case(char*** raw_text, int str_count, char*** cultivated_text) {
	int cultivated_str_count = 0;
	*cultivated_text = (char**)malloc(sizeof(char*));
	if (*cultivated_text == NULL) {
		return -1;
	}

	for (int i = 0; i < str_count; i++) {
		char** reserv_ptr_1 = (char**)realloc((*cultivated_text), sizeof(char*) * (i + 1));
		if (reserv_ptr_1 == NULL) {
			free_memory(cultivated_text, cultivated_str_count);
			return -1;
		} else {
			*cultivated_text = reserv_ptr_1;
		}

		(*cultivated_text)[i] = (char*)malloc(sizeof(char));
		if ((*cultivated_text)[i] == NULL) {
			free_memory(cultivated_text, cultivated_str_count);
			return -1;
		}

		char* reserv_ptr_2 = (char*)realloc((*cultivated_text)[i], sizeof(char) * (strlen((*raw_text)[i]) + 1));
		if (reserv_ptr_2 == NULL) {
			free_memory(cultivated_text, cultivated_str_count);
			return -1;
		} else {
			(*cultivated_text)[i] = reserv_ptr_2;
		}

		int j = 0;
		for (; (*raw_text)[i][j] != '\0'; j++) {
			(*cultivated_text)[i][j] = tolower((*raw_text)[i][j]);
		}
		(*cultivated_text)[i][j] = '\0';
		cultivated_str_count++;
	}

	return cultivated_str_count;
}

void free_memory(char*** text, int str_count) {
	for (int i = 0; i < str_count; i++) {
		free((*text)[i]);
	}
	free((*text));
}
