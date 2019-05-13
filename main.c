#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define TAPE_LENGTH 30000
#define MAX_BYTE ((1<<8) - 1)

typedef unsigned char byte;

char* program = NULL;
long program_len;
byte tape[TAPE_LENGTH];
int tape_pos = 0;
FILE* file = NULL;

int run(long program_pos) {
	while (program_pos < program_len) {
		switch (program[program_pos]) {
			case '+':
				if (tape[tape_pos] == MAX_BYTE) {
                    tape[tape_pos] = 0;
				} else {
				    tape[tape_pos]++;
				}
				break;
			case '-':
			    if (tape[tape_pos] == 0) {
			        tape[tape_pos] = MAX_BYTE;
			    } else {
                    tape[tape_pos]--;
			    }
				break;
			case '>':
			    if (++tape_pos >= TAPE_LENGTH) {
			        tape_pos = 0;
			    }
				break;
			case '<':
				if (--tape_pos < 0) {
				    tape_pos = TAPE_LENGTH - 1;
				}
				break;
			case '.':
				printf("%c", (char)tape[tape_pos]);
				break;
			case ',': {
                unsigned char input;
                scanf("%c", &input);
                tape[tape_pos] = input;
				break;
			}
			case '[': {
				long new_pos = -1;
				while (tape[tape_pos]) {
					new_pos = run(program_pos + 1);
				}
				if (new_pos != -1) {
					program_pos = new_pos;
				} else {
					int open_loops = 1;
					while (open_loops) {
						if (++program_pos > program_len) {
							free(program);
							exit(-1);
						}
						if (program[program_pos] == '[') {
							open_loops++;
						} else if (program[program_pos] == ']') {
							open_loops--;
						}
					}
				}
				break;
			}
			case ']':
				return program_pos;
		    case '\n':
		    case '\t':
		    case ' ':
		        break;
			default:
				printf("ERROR: Unrecognized character: %c", program[tape_pos]);
				free(program);
				exit(-1);
		}
		program_pos++;
	}

	return 0;
}

int main(int argc, char** argv) {
	if (argc != 2 && argc != 3) {
		printf("Usage:\ncrainfubk <file>\ncrainfubk exec <brainfuck code>\n");
		return 1;
	}

	if (argc == 3) {
		if (strcmp(argv[1], "exec")) {
			printf("Usage:\ncrainfubk <file>\ncrainfubk exec <brainfuck code>\n");
			return 1;
		}

		program_len = strlen(argv[2]);
		program = (char*)malloc(program_len * (sizeof(char)));
		strcpy(program, argv[2]);

		run(0);
	} else {
		file = fopen(argv[1], "r");

		if (file == NULL) {
			printf("ERROR: Could not read file '%s'\n", argv[1]);
		}

		fseek(file, 0, SEEK_END);
		program_len = ftell(file);
		fseek(file, 0, SEEK_SET);

		program = (char*)malloc(program_len);
		fread(program, sizeof(char), program_len, file);

		fclose(file);

		run(0);
	}


	free(program);
}
