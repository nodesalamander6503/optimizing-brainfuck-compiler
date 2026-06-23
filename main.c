#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

enum op_type {
	op_type_inc,
	op_type_dec,
	op_type_add,
	op_type_lbrac,
	op_type_rbrac,
	op_type_mvl,
	op_type_mvr,
	op_type_mov,
	op_type_input,
	op_type_output,
};
struct op {
	enum op_type type;
	int v;
};

#define dbprintf(...) {}
//#define dbprintf(...) printf(__VA_ARGS__);

enum panics {
	panic_oom,
	panic_ips,
};
void panic(enum panics p) {
	fprintf(stderr, "PANIC!\n\t");
	switch(p) {
		case panic_oom: {
			fprintf(stderr, "OUT OF MEMORY\n");
		} break;
		case panic_ips: {
			fprintf(stderr, "ILLEGAL PROGRAM STRUCTURE\n");
		} break;
	}
	exit(1);
}

void optimize(struct op **, int *);

void compile(struct op *, int);

int main(int argc, char ** argv) {
	char * code = "-[------->+<]>-.-[->+++++<]>++.+++++++..+++.[->+++++<]>+.------------.+[->++<]>.---[----->+<]>-.+++[->+++<]>++.++++++++.+++++.--------.-[--->+<]>--.+[->+++<]>+.++++++++.+[++>---<]>-.";
	if(argc >= 2) {
		code = argv[1];
	}
	struct op * program = malloc(sizeof(struct op));
	if(program == NULL) { panic(panic_oom); } // this right here. stupidest possible panic :3
	int program_alloc = 1;
	int program_len = 0;
	// 1. parse
#define add_op(x) { \
	program_len ++; \
	if(program_alloc < program_len) {\
		program_alloc *= 2;\
		program = realloc(program, sizeof(struct op) * program_alloc);\
		if(program == NULL) { panic(panic_oom); }\
	}\
	program[program_len - 1].type = (x);\
	program[program_len - 1].v = 0;\
}
	for(int i = 0; code[i] != 0; i ++) {
		switch(code[i]) {
			case '+': add_op(op_type_inc);   break;
			case '-': add_op(op_type_dec);   break;
			case '[': add_op(op_type_lbrac); break;
			case ']': add_op(op_type_rbrac); break;
			case '<': add_op(op_type_mvl);   break;
			case '>': add_op(op_type_mvr);   break;
			case ',': add_op(op_type_input); break;
			case '.': add_op(op_type_output);break;
			default: break;
		};
	}
#undef add_op
	/*
	// 2. precompute jumps
	for(int i = 0; i < program_len; i ++) {
		switch(program[i].type) {
			case op_type_lbrac: {
				int count = 1;
				int j = i;
				while(count > 0) {
					j ++;
					if(j >= program_len) { panic(panic_ips); }
					if(program[j].type == op_type_lbrac) { count ++; }
					if(program[j].type == op_type_rbrac) { count --; }
				}
				program[i].v = j;
			} break;
			case op_type_rbrac: {
				int count = 1;
				int j = i;
				while(count > 0) {
					j --;
					if(j < 0) { panic(panic_ips); }
					if(program[j].type == op_type_lbrac) { count --; }
					if(program[j].type == op_type_rbrac) { count ++; }
				}
				program[i].v = j;
			} break;
			default: break;
		}
	}
	*/
	// 3. slight optimizations
	optimize(&program, &program_len);
	// 4. compile
	compile(program, program_len);
	// 5. eval
	unsigned char * mem = calloc(1024, 1); // zeroed
	if(mem == NULL) { panic(panic_oom); }
	//for(int i = 0; i < 1024; i ++) { mem[i] = 0; }
	int pc = 0;
	int mp = 0;
	//int K = 200;
	while(pc < program_len) {
		//K --; if(K <= 0) { break; }
		switch(program[pc].type) {
			case op_type_inc: {
				mem[mp] ++;
				dbprintf("inc\n");
			} break;
			case op_type_dec: {
				mem[mp] --;
				dbprintf("dec\n");
			} break;
			case op_type_add: {
				mem[mp] += program[pc].v;
				dbprintf("add %d\n", program[pc].v);
			} break;
			case op_type_mvl: {
				mp --;
				if(mp < 0) {
					mp += 1024;
				}
				dbprintf("slide left\n");
			} break;
			case op_type_mvr: {
				mp ++;
				if(mp >= 1024) {
					mp -= 1024;
				}
				dbprintf("slide right\n");
			} break;
			case op_type_mov: {
				mp += program[pc].v;
				if(mp < 0) {
					mp += 1024;
				}
				if(mp >= 1024) {
					mp -= 1024;
				}
				dbprintf("slide\n");
			} break;
			case op_type_lbrac: {
				if(mem[mp] != 0) {
					dbprintf("fail right jump\n");
					break;
				}
				// /*
				int count = 1;
				while(count > 0) {
					pc ++;
					if(pc >= program_len) { panic(panic_ips); }
					if(program[pc].type == op_type_lbrac) { count ++; }
					if(program[pc].type == op_type_rbrac) { count --; }
				}
				//*/
				//pc = program[pc].v;
				dbprintf("succeed right jump\n");
			} break;
			case op_type_rbrac: {
				if(mem[mp] == 0) {
					dbprintf("fail left jump\n");
					break;
				}
				// /*
				int count = 1;
				while(count > 0) {
					pc --;
					if(pc < 0) { panic(panic_ips); }
					if(program[pc].type == op_type_lbrac) { count --; }
					if(program[pc].type == op_type_rbrac) { count ++; }
				}
				//*/
				//pc = program[pc].v;
				dbprintf("succeed left jump\n");
			} break;
			case op_type_input: {
				int c = getchar();
				mem[mp] = (c == EOF) ? 0 : (unsigned char) c;
			} break;
			case op_type_output: {
				dbprintf("->");
				printf("%c", mem[mp]);
				fflush(stdout);
			} break;
		}
		pc ++;
	}
	printf("\n");
	free(program);
	free(mem);
	return 0;
}

/**
 * Expects a, a_len to actually be defined
 * It'll fill B, B_len in on it's own; expects these to be not yet initialized
 * It'll annotate `edited` and will always write there
 */
void optimize_once(struct op * a, int a_len, struct op ** B, int * B_len, bool * edited) {
	struct op * b = NULL;
	int b_len = 0;
	int b_alloc = 4;
	b = malloc(sizeof(struct op) * b_alloc);
	if(b == NULL) { panic(panic_oom); }
	*edited = 0;
#define add_to_b(_type, _v) { \
	b_len ++; \
	if(b_alloc < b_len) {\
		b_alloc *= 2;\
		if(b_alloc == 0) {\
			b_alloc = 4;\
		}\
		b = realloc(b, sizeof(struct op) * b_alloc);\
		if(b == NULL) { panic(panic_oom); }\
	}\
	b[b_len - 1].type = (_type);\
	b[b_len - 1].v = (_v);\
}
	int i = 0;
	while(i < a_len) {
		bool foldable = 0;
		enum op_type fold_to; // what it folds into
		enum op_type fold_of = a[i].type; // what we're trying to fold
		int  fold_arg = 0;
		int  fold_mod = 0;
		// bool fold_use = 0; // will be generalized to enum if needed later
		switch(fold_of) {
			case op_type_inc: {
				foldable = 1;
				fold_to = op_type_add;
				fold_mod = 1;
			} break;
			case op_type_dec: {
				foldable = 1;
				fold_to = op_type_add;
				fold_mod = -1;
			} break;
			case op_type_mvl: {
				foldable = 1;
				fold_to = op_type_mov;
				fold_mod = 1;
			} break;
			case op_type_mvr: {
				foldable = 1;
				fold_to = op_type_mov;
				fold_mod = -1;
			} break;
			default: {
				foldable = 0;
				// this is degenerate-ish case, i guess; i technically could interrupt the for loop here with `continue;` but i want to not do that, in case i want to add more stuff later
			} break;
		}
		if(!foldable) {
			// just append to b
			add_to_b(a[i].type, a[i].v);
			i ++;
			continue;
		}
		int j = i;
		while(j < a_len && a[j].type == fold_of) {
			fold_arg += fold_mod;
			j ++;
		}
		if((j - i) > 1) { *edited = 1; }
		i = j;
		// now append new folded thing to b instead
		add_to_b(fold_to, fold_arg);
	}
	// realloc to get rid of unneeded allocated memory
	{
		struct op * tmp = realloc(b, sizeof(struct op) * b_len);
		if(tmp != NULL) { b = tmp; }
	}
	*B = b;
	*B_len = b_len;
}

void optimize(struct op ** p, int * p_len) {
	struct op * B;
	int B_len;
	bool edited = 0;
	optimize_once(*p, *p_len, &B, &B_len, &edited);
	free(*p);
	*p = B;
	*p_len = B_len;
}

void compile(struct op * program, int len) {
	FILE * file = fopen("compiled-bf.c", "w+");
	fprintf(file, "#include <stdio.h>\n#include <stdlib.h>\nint main(int argc, char ** argv) {\nchar * tape = calloc(1024, 1);\nint tp = 0;\nint c;\n");
	int label_counter = 0;
	int * label_stack = NULL;
	int label_stack_height = 0;
	int label_stack_allocated = 0;
#define append_to_label_stack(label) { \
	if(label_stack_height >= label_stack_allocated) { \
		int realloc_to = label_stack_allocated == 0 ? 2 : label_stack_allocated * 2; \
		label_stack = realloc(label_stack, sizeof(int) * realloc_to); \
		if(label_stack == NULL) { panic(panic_oom); } \
		label_stack_allocated = realloc_to; \
	} \
	label_stack[label_stack_height++] = (label); \
}
#define pop_from_label_stack() (label_stack[--label_stack_height])
	for(int i = 0; i < len; i ++) {
		switch(program[i].type) {
			case op_type_inc: {
				fprintf(file, "tape[tp]++;\n");
			} break;
			case op_type_dec: {
				fprintf(file, "tape[tp]--;\n");
			} break;
			case op_type_add: {
				fprintf(file, "tape[tp]+=%d;", program[i].v);
			} break;
			case op_type_mvl: {
				fprintf(file, "tp--;\n");
			} break;
			case op_type_mvr: {
				fprintf(file, "tp++;\n");
			} break;
			case op_type_mov: {
				fprintf(file, "tp+=%d;", program[i].v);
			} break;
			case op_type_lbrac: {
				fprintf(file, "_l%d:\n", label_counter);
				fprintf(file, "if(tape[tp] == 0) { goto _r%d; }\n", label_counter);
				append_to_label_stack(label_counter);
				label_counter ++;
			} break;
			case op_type_rbrac: {
				int x = pop_from_label_stack();
				fprintf(file, "_r%d:\n", x);
				fprintf(file, "if(tape[tp] != 0) { goto _l%d; }\n", x);
			} break;
			case op_type_input: {
				fprintf(file, "int c = getchar();\n");
				fprintf(file, "tape[tp] = (c == EOF) ? 0 : (unsigned char) c;\n");
			} break;
			case op_type_output: {
				fprintf(file, "printf(\"%%c\", tape[tp]);");
			} break;
		}
	}
	fprintf(file, "return 0;\n}\n\n");
	fflush(file);
	fclose(file);
}

