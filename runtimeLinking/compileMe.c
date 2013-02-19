#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
int main (int argc, char **argv){
	const char * populate = "#include <stddef.h>\n#include <math.h>\n" "void populateVector( double *Vector , size_t length) {\n" "\tfor ( size_t %s = 0 ; %s < length ; %s++ ){\n" "\t\tVector[%s] =  %s;\n" "\t}\n" "}\n";
	if ( argc == 3 ){
		FILE * file = fopen("foo.c", "w");
		if ( file == NULL ){
			return 0;
		}
		fprintf( file , populate , argv[1] , argv[1] , argv[1] , argv[1] , argv[2] );
		fflush(file);
		fclose(file);
		int retval1 = WEXITSTATUS (system("clang -shared -o libhello.so foo.c"));
		int retval2 = 0;//WEXITSTATUS (system("clang -shared -lm -o libhello.so foo.o"));
		if (retval1 == 0  && retval2 == 0) {
			char * dlErrMsg;
			dlErrMsg = dlerror();
			if (dlErrMsg != NULL){
				printf("%s\n", dlErrMsg);
				return 0;
			}
			printf("success\n");
			size_t length =10;
			double *Vector = calloc(1000, sizeof(double));
			void (*func)(double *, size_t);
			void *dyn = dlopen("/home/torje/libhello.so", RTLD_NOW);
			if (dlErrMsg != NULL){
				printf("%s\n", dlErrMsg);
				return 0;
			}
			
			func = dlsym(dyn, "populateVector");
			if (dlErrMsg != NULL){
				printf("%s\n", dlErrMsg);
				return 0;
			}
			printf("func = %ld\n", (size_t)func);
			printf("calling dynamically loaded func\n");
			func(Vector, length);
			printf("returned from dyn\n");
			for ( int i = 0 ; i < length ; i ++ ){
				printf("%lf ", Vector[i]);
			}
			printf("\n");

		}
		return 0;
	}
	return 0;
}
