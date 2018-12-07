#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_OP 10 // numarul maxim de operatii
#define MAX_CHAR 21 // numarul maxim de caractere al unei operatii (20+1)

char *strlwr(char *);
char **commands(char **, int, char **, int *);
char *split(char *, int, int);
int char_to_int(char *, int);
char **prep_wrap(char **, char*, int *);
char **wrap(char **, int *, int);
char **prep_align_left(char **, char *, int *);
void align_left(char **, int, int);
char **prep_align_right(char **, char *, int *);
void align_right(char **, int, int, int);
char **rm_trailing_whitespace(char **, int);

int main(int argc, char *argv[]) {
  char buf[1000],               // buffer folosit pentru citirea din fisier
      //original[1000][1000],     // textul original, linie cu linie
      result[1000][1000];      // textul obtinut dupa aplicarea operatiilor
  char **original = (char **) calloc(1000, sizeof(char *));
  char **clona_original = (char **) calloc(1000, sizeof(char *));
  char **operations = (char **) calloc(MAX_OP + 1, sizeof(char *));
  int original_line_count = 0,  // numarul de linii din input file
      result_line_count = 0,    // numarul de linii din output file
      i,
      no_arguments;	// numarul de argumente
  if((original == NULL) || (clona_original == NULL)){
     return -1;
  }
  for(i = 0; i < 1000; i++){
     original[i] = (char*) calloc(1000, sizeof(char));
     clona_original[i] = (char*) calloc(1000, sizeof(char));
     if((original[i] == NULL) || (clona_original == NULL)){
	return -1;
     }
  }
  if(operations == NULL){
     return -1;
  }
  for(i = 0; i < MAX_OP; i++){
     operations[i] = (char*) calloc(MAX_CHAR, sizeof(char));
     if(operations[i] == NULL){
	return -1;
     }
  }
  if (argc != 4) {  // numar invalid de argumente
    fprintf(stderr,
            "Usage: %s operations_string input_file output_file\n",
            argv[0]);
    return -1;
  }

  // deschidem input file pentru citire
  FILE *input_file = fopen(argv[2], "r");

  if (input_file == NULL) {
    fprintf(stderr, "File \"%s\" not found\n", argv[2]);
    return -2;
  }

  // citim din fisier linie cu linie
  while (fgets(buf, 1000, input_file)) {
    strcpy(original[original_line_count], buf);
    original_line_count++;
  }

  fclose(input_file);

  /*
   * TODO apply operations in operation_string (argv[1]) one by one and save
   * the result in the 'result' matrix.
   * FREE PENTRU OPERATIONS SI ALTELE
   * GRIJA LA TRAILING WHITESPACE - facut
   */
  for (i = 0; i < original_line_count; i++){
     strcpy(clona_original[i], original[i]);
  }
  // impartim sirul de operatii
  argv[1] = strlwr(argv[1]);
  i = 0;
  operations[i] = strtok(argv[1], ",");
  // parcurgem fiecare token
  while ( operations[i] != NULL ) {
     if (i >= MAX_OP){
	printf("Too many operations! Only the first 10 will be applied.\n");
	break;
     }
     i++;
     operations[i] = strtok(NULL, ",");
  }
  // eliminam starting whitespace ul fiecarei operatii daca exista
  no_arguments = i--;
  while (i >= 0){
     while ( operations[i][0] == ' ' ){
        operations[i]++;
     }
     //printf( "%s\n", operations[i]);
     i--;
  }

  clona_original = commands(operations, no_arguments, clona_original, &original_line_count);
  if (clona_original != NULL){
     for (i = 0; i <= original_line_count; i++){
        strcpy(original[i], clona_original[i]);
     }
     original = rm_trailing_whitespace(original, original_line_count);
  }
  //original = rm_trailing_whitespace(original, original_line_count);
  for (i = 0; i <= original_line_count; i++) {
     strcpy(result[i], original[i]);
  }
  result_line_count = original_line_count;

  // deschidem output file pentru scriere
  FILE *output_file = fopen(argv[3], "w");

  if (output_file == NULL) {
    fprintf(stderr, "Could not open or create file \"%s\"\n", argv[3]);
    return -2;
  }

  // scriem rezultatul in output file
  for (i = 0; i <= result_line_count; i++) {
    fputs(result[i], output_file);
  }

  fclose(output_file);
}

char *strlwr(char *string)
{
  unsigned char *c = (unsigned char *)string;

  while (*c) {
     *c = tolower((unsigned char)*c);
      c++;
  }
  return string;
}

char **commands(char **operations, 
	      int no_arguments,
	      char **original,
	      int *original_line_count)
{ 
  int i;
  for (i = 0; i < no_arguments; i++){
     switch (operations[i][0]){
	case 'w':
	   original = prep_wrap(original, operations[i], original_line_count);
	   break;
	case 'c':
	   break;
	case 'l':
	   original = prep_align_left(original, operations[i], original_line_count);
	   break;
	case 'r':
	   original = prep_align_right(original, operations[i], original_line_count);
	   break; 	
	case 'j':
	   printf("Justify\n");
	   break;
	case 'p':
	   printf("Paragraph\n");
	   break;
	case 'i':
	   printf("List\n");
	   break;
	case 'o':
	   printf("Ord List\n");
	   break;
	default:
	   printf("Invalid operation!\n");
	   return NULL;
     }
     if (original == NULL){
	return NULL;
     }
  }
  return original;
}

char **prep_wrap(char ** original, char *operations, int *original_line_count)
{
  int k = 0;
  char *token = (char *) calloc(MAX_CHAR, sizeof(char));
  char *number = (char *) calloc(MAX_CHAR, sizeof(char));
  token = strtok(operations, " ");
  while ( token != NULL ){
     k++;
     if (k == 2){
        number = token;
     }
     if (k > 2){
        printf("Invalid operation!\n");
        return NULL;
     }
     token = strtok(NULL, " ");
  }
  int max_line_length = char_to_int(number, strlen(number));
  if (max_line_length == -1){
     printf("Invalid operation!\n");
     return NULL; 
  }
  original = wrap(original, original_line_count, max_line_length);
  if (original == NULL){
     return NULL;
  }
  return original;
}

char *split(char *ar, int token_needed, int max_tokens)
{
  char *token, *token_to_return;
  char *array = (char *) calloc(20, sizeof(char));
  strcpy(array,ar);
  int i=0;
  token = strtok(array, " ");
  while ( token != NULL ) {
     i++;
     if (i == token_needed){
        token_to_return = token;
     }
     if (i > max_tokens){
        printf("Invalid operation!\n");
	return NULL;
     }
     token = strtok(NULL, " ");
  }
  puts(token_to_return);
  free(array);
  return token_to_return;
}

int char_to_int(char *num_char, int length)
{
  int pow_10 = 1, num_int = 0;
  length--;
  while ( length >= 0 ){
     if ( !isdigit(num_char[length]) ){
        return -1;
     }
     num_int += ( num_char[length--] - '0' ) * pow_10;
     pow_10 *= 10;
  }
  return num_int;
}

char **wrap(char **original, int *original_line_count, int max_line_length)
{
  int i, char_count, line;
  char *token;
  char **text = (char **) calloc(1000, sizeof(char *));
  if (text == NULL){
     return original;
  }
  for (i = 0; i < 1000; i++){
     text[i] = (char *) calloc(1000, sizeof(char));
     if(text[i] == NULL){
	return original;
     }
  }
  line = 0;
  char_count = 0;
  for (i = 0; i < *original_line_count; i++){
     while ( original[i][0] == ' ' ){
        original[i]++;
     }
     // daca urmeaza alt paragraf
     if (original[i][0] == '\n' ){
	line += 2;
	char_count = 0;
	continue;
     }
     token = strtok(original[i], " \n");
     while ( token != NULL ){
	if (strlen(token) > max_line_length){
	   printf("Cannot wrap!\n");
	   return NULL;
	}
	if (char_count == 0){
	   strcpy(text[line], token);
	   char_count = strlen(token);
	}
	else{
	   if ((strlen(token) + char_count + 1) > max_line_length){
	      strcpy(text[++line], token);
	      char_count = strlen(token);
	   }
	   else{
	      strcat(text[line], " ");
	      strcat(text[line], token);
	      char_count += strlen(token);
	      char_count ++;
	   }
	}
	token = strtok(NULL, " \n");
     }
  }
  for (i = 0; i <= line; i++){
	      strcat(text[i],"\n");
  	   }
  *original_line_count = line;
  return text;
}

char **prep_align_left(char **original, char *operations, int *original_line_count){
  int start_line, end_line, k = 0; 
  char *start_line_string, *end_line_string = NULL, 
       *token = (char *) calloc(MAX_CHAR, sizeof(char));
  token = strtok(operations, " ");
  while ( token != NULL ){
     k++;
     if (k == 2){
	start_line_string = token;
     }
     if (k == 3){
	end_line_string = token;
     }
     if (k > 3){
	printf("Invalid operation!\n");
	return NULL;
     }
     token = strtok(NULL, " ");
  }
  free(token);
  if (k == 1){
     start_line = 0;
     end_line = *original_line_count;
  }
  else{
     start_line = char_to_int(start_line_string, strlen(start_line_string));
     if (start_line == -1){
	printf("Invalid operation!\n");
	return NULL;
     }
     if (end_line_string == NULL){
	end_line = *original_line_count;
     }
     else{
	end_line = char_to_int(end_line_string, strlen(end_line_string));
	if (end_line == -1){
	   printf("Invalid operation!\n");
	   return NULL;
        }
	if (end_line < start_line){
	   printf("Invalid operation!\n");
           return NULL;
	}
     }
  }
  align_left(original, start_line, end_line);
  return original;
}

void align_left(char **original, int start_line, int end_line)
{
  int i = start_line;
  for (; i <= end_line; i++){
     while ( original[i][0] == ' ' ){
        original[i]++;
     }
  }
}

char **prep_align_right(char **original, char *operations, int *original_line_count){
  int start_line, end_line, k = 0; 
  char *start_line_string, *end_line_string = NULL, 
       *token = (char *) calloc(MAX_CHAR, sizeof(char));
  token = strtok(operations, " ");
  while ( token != NULL ){
     k++;
     if (k == 2){
	start_line_string = token;
     }
     if (k == 3){
	end_line_string = token;
     }
     if (k > 3){
	printf("Invalid operation!\n");
	return NULL;
     }
     token = strtok(NULL, " ");
  }
  free(token);
  if (k == 1){
     start_line = 0;
     end_line = *original_line_count - 1;
  }
  else{
     start_line = char_to_int(start_line_string, strlen(start_line_string));
     if (start_line == -1){
	printf("Invalid operation!\n");
	return NULL;
     }
     if (end_line_string == NULL){
	end_line = *original_line_count - 1;
     }
     else{
	end_line = char_to_int(end_line_string, strlen(end_line_string));
	if (end_line == -1){
	   printf("Invalid operation!\n");
	   return NULL;
        }
	if (end_line < start_line){
	   printf("Invalid operation!\n");
           return NULL;
	}
	if (end_line >= *original_line_count){
	   end_line = *original_line_count - 1;
	}
     }
  }
  int max_length = strlen(original[0]);
  for (k = 1; k < *original_line_count; k++){
     if (strlen(original[k]) > max_length){
	max_length = strlen(original[k]);
     }
  }
  align_right(original, start_line, end_line, max_length);
  return original;
}

void align_right(char **original, int start_line, int end_line, int max_length){
  int i, spaces;
  char *line = (char *) calloc(1000, sizeof(char));
  for (i = start_line; i <= end_line; i++){
     spaces = max_length - strlen(original[i]);
     while ( spaces ){
	strcpy(line, original[i]);
	strcpy(original[i], " ");
	strcat(original[i], line);
	spaces--;
     }
  }
  free(line);
}

char **rm_trailing_whitespace(char **original, int original_line_count){
  int i, j;
  for (i = 0; i <= original_line_count; i++){
     j = strlen(original[i]);
     j -= 2;
     while ( isspace(original[i][j]) ){
	original[i][j] = '\0';
	j--;
     }
     if (j != strlen(original[i]) - 2){
	strcat(original[i], "\n");
     }
  }
  return original;
}
