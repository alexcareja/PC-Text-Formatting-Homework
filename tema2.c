#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_OP 10 // numarul maxim de operatii
#define MAX_CHAR_OP 21 // numarul maxim de caractere al unei operatii (20+1)
#define MAX_CHAR_LINE 1000 // numarul maxim de caractere de pe o linie

// Declarare prototipuri de functii
// Functiile prep_"/\/\/\" au un rol pregatitor, inainte de aplicarea fiecarei 
// operatii. De obicei este facuta transformarea parametrilor, tratarea 
// cazurilor speciale si apelarea functiei care face modificarile cerute
// Fiecare functie are si o mica descriere pe care o gasiti acolo unde am scris
// functia in sine
char **commands(char **, int, char **, int *);
int char_to_int(char *);
char **prep_wrap(char **, char *, int *);
char **wrap(char **, int *, int);
char **prep_center(char **, char *, int );
void center(char **, int, int, int);
char **prep_align_left(char **, char *, int );
void align_left(char **, int, int);
char **prep_align_right(char **, char *, int );
void align_right(char **, int, int, int);
char **prep_justify(char **, char *, int);
char **justify(char **, int, int);
char **prep_paragraph(char **, char *, int);
void paragraph(char **, int, int, int);
char **prep_list(char **, char *, int);
void num_list(char **, char *, int, int);
void alphabetic_list(char **, char, char *, int, int);
void bullet_list(char **, char *, int, int);
char **prep_olist(char **, char *, int);
void order(char **, int, int, char);
char **rm_trailing_whitespace(char **, int);

int main(int argc, char *argv[]) {
  char buf[MAX_CHAR_LINE], // buffer folosit pentru citirea din fisier
      result[MAX_CHAR_LINE][MAX_CHAR_LINE]; // textul obtinut dupa aplicarea
					    // operatiilor
  char **original = (char **) calloc(MAX_CHAR_LINE, sizeof(char *)); // textul 
								     // original
  char **clona_original = (char **) calloc(MAX_CHAR_LINE, sizeof(char *));// un
   // vector de stringuri pe care aplicam operatiile pentru a nu altera ORIGINAL
   // in eventualitatea unui caz special in care trebuie sa returnam sirul ORI-
   // GINAL 
  char **operations = (char **) calloc(MAX_OP + 1, sizeof(char *)); // vector de 
					//stringuri in care memorez operatiile
  // Prin "string" intelegem vector de char-uri (va mai aparea, si stiu ca nu 
  // am declarat variabile de tip string, dar imi este mai usor sa ma refer la 
  // ele in acest mod)
  int original_line_count = 0, // numarul de linii din input file
      result_line_count = 0, // numarul de linii din output file
      clona_original_line_count = 0, // line count care tine de clona 
				     // originalului
      i, // contor
      no_arguments;	// numarul de argumente
  if((original == NULL) || (clona_original == NULL)){
     return -1;
  }

  // alocarea dinamica pentru vectorii de stringuri

  for(i = 0; i < MAX_CHAR_LINE; i++){
     original[i] = (char*) calloc(MAX_CHAR_LINE, sizeof(char));
     clona_original[i] = (char*) calloc(MAX_CHAR_LINE, sizeof(char));
     if((original[i] == NULL) || (clona_original == NULL)){
	return -1;
     }
  }
  if(operations == NULL){
     return -1;
  }
  for(i = 0; i < MAX_OP; i++){
     operations[i] = (char*) calloc(MAX_CHAR_OP, sizeof(char));
     if(operations[i] == NULL){
	return -1;
     }
  }
  if (argc != 4) { // numar invalid de argumente
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
  while (fgets(buf, MAX_CHAR_LINE, input_file)) {
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
  // copiem continutul originalului in clona
  for (i = 0; i < original_line_count; i++){
     strcpy(clona_original[i], original[i]);
  }
  clona_original_line_count = original_line_count;

  // impartim sirul de operatii dupa ','
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
     operations[i][0] = tolower((unsigned char)operations[i][0]);
     i--;
  }

  // apelam functia commands pentru aplicarea operatiilor
  clona_original = commands(operations, no_arguments, clona_original,
			    &clona_original_line_count);
  
  // daca functia commands nu a returnat NULL inseamna ca operatiile au fost 
  // valide si ca in clona_original avem textul final. In caz contrar, adica
  // daca a returnat NULL, atunci inseamna ca a aparut un caz special si ca 
  // trebuie returnat textul original
  if (clona_original != NULL){
     for (i = 0; i <= clona_original_line_count; i++){
        strcpy(original[i], clona_original[i]);
     }
     original_line_count = clona_original_line_count;
     original = rm_trailing_whitespace(original, original_line_count);
  }

  // copiem in matricea result textul final
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

  // dezalocam spatiul pentru variabilele alocate dinamic
  for (i = 0; i <= MAX_CHAR_LINE; i++){
     free(original[i]);
  }
  free(original);
  fclose(output_file);
  return 0;
}

// functia commands lucreaza pe vectorul de stringuri operations unde sunt sal-
// vate operatiile, si in functie de prima litera din sir, va fi apelata functia
// pentru operatia corespunzatoare
// Se returneaza NULL atunci cand original nu trebuie modificat deloc
// Se returneaza textul final in cazuri normale
char **commands(char **operations, int no_arguments, char **original,
	        int *original_line_count)
{ 
  int i;
  for (i = 0; i < no_arguments; i++){
     switch (operations[i][0]){
	case 'w':
	   original = prep_wrap(original, operations[i], original_line_count);
	   break;
	case 'c':
	   original = prep_center(original, operations[i], *original_line_count);
	   break;
	case 'l':
	   original = prep_align_left(original, operations[i], *original_line_count);
	   break;
	case 'r':
	   original = prep_align_right(original, operations[i], *original_line_count);
	   break; 	
	case 'j':
	   original = prep_justify(original, operations[i], *original_line_count);
	   break;
	case 'p':
	   original = prep_paragraph(original, operations[i], *original_line_count);
	   break;
	case 'i':
	   original = prep_list(original, operations[i], *original_line_count);
	   break;
	case 'o':
	   original = prep_olist(original, operations[i], *original_line_count);
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

// Pregatirea pentru wrap. Avem nevoie de numai un parametru dupa W, si acela
// sa fie numar intreg pozitiv. Tranfromarea lui o fac cu functia char_to_int
// (se aplica pentru toate numerele de genul)
// Se returneaza NULL atunci cand original nu trebuie modificat deloc
// Se returneaza textul final in cazuri normale
char **prep_wrap(char ** original, char *operations, int *original_line_count)
{
  int k = 0;
  char *token = (char *) calloc(MAX_CHAR_OP, sizeof(char));
  char *number = NULL;
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
  free(token);
  if (number == NULL){
     printf("Invalid operation!\n");
     return NULL; 
  }
  int max_line_length = char_to_int(number);
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

// Functia char_to_int returneaza numarul din stringul trimis ca parametru, sau
// NULL daca sirul contine alte caractere in afara de cifre. (inclusiv semnul 
// '-' pentru numerele negative care fac parte din cazurile speciale)
int char_to_int(char *num_char)
{
  int pow_10 = 1, num_int = 0, length = strlen(num_char);
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

// In wrap am declarat un vector de stringuri paragraph in care memoram pe cate 
// o linie fiecare paragraf din original. Aceasta memorare se face cu strcpy 
// pentru prima linie din paragraf si cu strcat pentru celelalte. Mereu adaugam
// si un spatiu intre linii consecutive din paragraf. Inainte de aceasta, insa, 
// eliminam trailing whitespace-ul liniilor pentru concatenarea lor corecta.
// Verificam daca exista vreun cuvant mai mare decat max_line_length si returnam
// NULL daca gasim. Eliminam toate '\n' care exista pentru a putea avea 
// consecventa dorita pentru functia isspace. Ideea de rezolvare este sa verific
// daca caracterul de pe pozitia max_line_length - 1 este un spatiu. Daca da, 
// atunci toate cele max_line_length - 1 caractere sunt copiate intr-o linie
// din matricea clona, se adauga '\n' la finalul liniei, pregatim urmatoarea
// linie din clona (i ++;) si crestem pointerul paragraph[j] pentru ca el sa 
// pointeze catre urmatorul caracter dupa spatiul gasit de noi. Daca nu gasim
// spatiu din prima, atunci cautam in stanga pana cand gasim un spatiu si
// efectuam aceeasi operatie. In final modificam si valoarea variabilei 
// original_line_count din main
char **wrap(char **original, int *original_line_count, int max_line_length)
{
  int i, j, k, new_paragraph, no_paragraphs, line_length; 
  char **paragraph = (char **) calloc(MAX_CHAR_LINE, sizeof(char *));
  char **clona = (char **) calloc(MAX_CHAR_LINE, sizeof(char *));
  char * token;
  if ((paragraph == NULL) || (clona == NULL)){
     return original;
  }
  for (i = 0; i <= *original_line_count; i++){
     paragraph[i] = (char *) calloc(MAX_CHAR_LINE, sizeof(char));
     clona[i] = (char *) calloc(MAX_CHAR_LINE, sizeof(char));
     if (paragraph[i] == NULL){
	return original;
     }
  }
  for (i = 0; i <= MAX_CHAR_LINE; i++){
     clona[i] = (char *) calloc(MAX_CHAR_LINE, sizeof(char));
     if (clona[i] == NULL){
	return original;
     }
  rm_trailing_whitespace(original, *original_line_count);
  }
  // copiem liniile dintr-un paragraf intr-o linie in variabila paragraph
  for (i = 0, j = 0, new_paragraph = 1; i < *original_line_count; i++){
     // daca urmeaza alt paragraf
     if (original[i][0] == '\n' ){
	j++;
	new_paragraph = 1;
	continue;
     }
     if (new_paragraph){
	new_paragraph = 0;
	strcpy(paragraph[j], original[i]);
	strcat(paragraph[j], " ");
     }
     else{
	while (original[i][0] == ' '){
	   original[i]++;
	}
	strcat(paragraph[j], original[i]);
	strcat(paragraph[j], " ");
     }
     
  }
  // verificam daca avem un cuvant mai mare decat max_line_length
  for (i = 0; i < *original_line_count; i++){
     // daca urmeaza alt paragraf
     if (original[i][0] == '\n' ){
	continue;
     }
     token = strtok(original[i], " \n");
     while ( token != NULL ){
	if (strlen(token) > max_line_length){
	   printf("Cannot wrap!\n");
	   return NULL;
	
	}
	token = strtok(NULL, " \n");
     }
  }
  no_paragraphs = j;
  // eliminare '\n'
  while(j >= 0){
     i = 0;
     while( paragraph[j][i] != '\0'){
	if(paragraph[j][i] == '\n'){
	   memmove(paragraph[j] + i, paragraph[j] + i + 1, 700);
	}
	i++;
     }
     j--;
  }
  // impartim liniile in clona
  for (j = 0, i = 0; j <= no_paragraphs; j++){
     line_length = strlen(paragraph[j]);
     while(line_length > max_line_length){
	k = 1;
	while ( k ){
	   if(isspace(paragraph[j][max_line_length - k + 1])){
	      strncpy(clona[i], paragraph[j], max_line_length -k + 1);
	      strcat(clona[i], "\n");
	      i++;
	      paragraph[j] += max_line_length - k + 2;
	      break;
	   }
	   k++;
	}
	line_length = strlen(paragraph[j]);
     }
     if (line_length){
	strcpy(clona[i], paragraph[j]);
	strcat(clona[i], "\n");
	i++;
     }
     if (j < no_paragraphs){
        strcpy(clona[i], "\n");
        i++;
     }
  }
  *original_line_count = i ;
  return clona;
}

// Pregatirea pentru center. Avem 2 parametrii optionali care trebuie sa fie
// numere intregi pozitive. Tranfromarea: char_to_int
// Cautam cea mai lunga linie din original, si transmitem lungimea ei ca 
// parametru in functia center
// Se apeleaza center(...)
// Se returneaza NULL atunci cand original nu trebuie modificat deloc
// Se returneaza textul final in cazuri normale
char **prep_center(char **original, char *operations, int original_line_count){
  int start_line, end_line, k = 0; 
  char *start_line_string, *end_line_string = NULL, 
       *token = (char *) calloc(MAX_CHAR_OP, sizeof(char));
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
     end_line = original_line_count - 1;
  }
  else{
     start_line = char_to_int(start_line_string);
     if (start_line == -1){
	printf("Invalid operation!\n");
	return NULL;
     }
     if (end_line_string == NULL){
	end_line = original_line_count - 1;
     }
     else{
	end_line = char_to_int(end_line_string);
	if (end_line == -1){
	   printf("Invalid operation!\n");
	   return NULL;
        }
	if (end_line < start_line){
	   printf("Invalid operation!\n");
           return NULL;
	}
	if (start_line > (original_line_count - 1)){
	   return original;
	}
	if (end_line >= original_line_count){
	   end_line = original_line_count - 1;
	}
     }
  }
  int max_length = strlen(original[0]);
  for (k = 1; k < original_line_count; k++){
     if (strlen(original[k]) > max_length){
	max_length = strlen(original[k]);
     }
  }
  center(original, start_line, end_line, max_length);
  return original;
}

// Pentru fiecare linie calculam diferenta dintre lungimea celei mai lungi linii
// si lungimea liniei curente, si adaugam (n + 1) / 2 spatii la inceputul liniei
void center(char **original, int start_line, int end_line, int max_length){
  int i, spaces;
  char *line = (char *) calloc(MAX_CHAR_LINE, sizeof(char));
  for (i = start_line; i <= end_line; i++){
     spaces = max_length - strlen(original[i]);
     spaces = (spaces + 1) / 2;
     while ( spaces ){
	strcpy(line, original[i]);
	strcpy(original[i], " ");
	strcat(original[i], line);
	spaces--;
     }
  }
  free(line);
}

// Pregatirea pentru align_left. Avem 2 parametrii optionali care trebuie sa fie
// numere intregi pozitive. Tranfromarea: char_to_int
// Se apeleaza align_left(...)
// Se returneaza NULL atunci cand original nu trebuie modificat deloc
// Se returneaza textul final in cazuri normale
char **prep_align_left(char **original, char *operations, int original_line_count){
  int start_line, end_line, k = 0; 
  char *start_line_string, *end_line_string = NULL, 
       *token = (char *) calloc(MAX_CHAR_OP, sizeof(char));
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
     end_line = original_line_count;
  }
  else{
     start_line = char_to_int(start_line_string);
     if (start_line == -1){
	printf("Invalid operation!\n");
	return NULL;
     }
     if (start_line > (original_line_count)){
	return original;
     }
     if (end_line_string == NULL){
	end_line = original_line_count;
     }
     else{
	end_line = char_to_int(end_line_string);
	if (end_line == -1){
	   printf("Invalid operation!\n");
	   return NULL;
        }
	if (end_line < start_line){
	   printf("Invalid operation!\n");
           return NULL;
	}
	if (end_line >= original_line_count){
	   end_line = original_line_count;
	}
     }
  }
  align_left(original, start_line, end_line);
  return original;
}

// Pentru fiecare linie in intervalul cerut, eliminam toate caracterele de tip
// spatiu de la inceputul vectorului. (fac asta prin a creste pointerul catre
// inceputul stringului)
void align_left(char **original, int start_line, int end_line)
{
  int i = start_line;
  for (; i <= end_line; i++){
     if (original[i][0] == '\n'){
	continue;
     }
     while ( isspace(original[i][0]) ){
        original[i]++;
     }
  }
}

// Pregatirea pentru align_right. Avem 2 parametrii optionali care trebuie sa 
// fie numere intregi pozitive. Tranfromarea: char_to_int
// Cautam cea mai lunga linie din original si transmitem lungimea ei ca parame-
// tru in functia align_right pentru a alinia toate liniile dupa ea
// Se apeleaza align_right(...)
// Se returneaza NULL atunci cand original nu trebuie modificat deloc
// Se returneaza textul final in cazuri normale
char **prep_align_right(char **original, char *operations, int original_line_count){
  int start_line, end_line, k = 0; 
  char *start_line_string, *end_line_string = NULL, 
       *token = (char *) calloc(MAX_CHAR_OP, sizeof(char));
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
     end_line = original_line_count - 1;
  }
  else{
     start_line = char_to_int(start_line_string);
     if (start_line == -1){
	printf("Invalid operation!\n");
	return NULL;
     }
     if (start_line > (original_line_count - 1)){
	return original;
     }
     if (end_line_string == NULL){
	end_line = original_line_count - 1;
     }
     else{
	end_line = char_to_int(end_line_string);
	if (end_line == -1){
	   printf("Invalid operation!\n");
	   return NULL;
        }
	if (end_line < start_line){
	   printf("Invalid operation!\n");
           return NULL;
	}
	if (end_line >= original_line_count){
	   end_line = original_line_count - 1;
	}
     }
  }
  int max_length = strlen(original[0]);
  for (k = 1; k < original_line_count; k++){
     if (strlen(original[k]) > max_length){
	max_length = strlen(original[k]);
     }
  }
  align_right(original, start_line, end_line, max_length);
  return original;
}

// Pentru fiecare linie in intervalul cerut calculam diferenta dinte max_length
// si lungimea liniei curente. Rezultatul obtinut este numarul de spatii pe care
// il adaugam la inceputul fiecari linii
void align_right(char **original, int start_line, int end_line, int max_length){
  int i, spaces;
  char *line = (char *) calloc(MAX_CHAR_LINE, sizeof(char));
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

// Pegatirea pentru justify. Avem 2 parametrii optionali care trebuie sa 
// fie numere intregi pozitive. Tranfromarea: char_to_int
// Aliniem la stanga textul si apelam justify.
char **prep_justify(char **original, char *operations, int original_line_count){
  int start_line, end_line, k = 0; 
  char *start_line_string, *end_line_string = NULL, 
       *token = (char *) calloc(MAX_CHAR_OP, sizeof(char));
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
  if (k < 2){
     start_line = 0;
     end_line = original_line_count;
  }
  else{
     start_line = char_to_int(start_line_string);
     if (k == 3){
	end_line = char_to_int(end_line_string);
     }
     else{
	end_line = original_line_count;
     }
  }
  align_left(original, start_line, end_line);
  return justify(original, start_line, end_line);
}

// Ideea de rezolvare a justify ului este de a cauta cea mai lunga linie din 
// original, iar apoi pentru fiecare linie (in afara de finalul de paragraf si
// liniile goale) sa calculam care este numarul de spatii care trebuie adaugate.
// Apoi am copiat intr-o variabila line continutul liniei si cu strtok am 
// numarat cate cuvinte sunt pe linie. Catul impartirii dintre numarul de spatii
// de adaugat si (numarul de cuvinte - 1) este numarul de spatii pe care il 
// adaugam intre oricare 2 cuvinte. In aditie, restul acelei impartiri este
// numarul de spatii pe care le adaugam in plus unul cate unul intre cuvinte in
// ordine de la stanga la dreapta.
char **justify(char **original, int start_line, int end_line){
  int k, i, line_length, max_length = strlen(original[start_line]), token_count, spaces, extra_spaces;
  char *line = (char *) calloc(MAX_CHAR_LINE, sizeof(char)), 
       *token = (char *) calloc(MAX_CHAR_LINE, sizeof(char));
  for (k = start_line + 1; k <= end_line ; k++){
     if (strlen(original[k]) > max_length){
	max_length = strlen(original[k]);
     }
  }
  for (k = end_line ; k >= start_line ; k--){
     // daca linia este un paragraf
     if (original[k][0] == '\n'){
	continue;
     }
     // daca este ultima linie dintr-un paragraf
     if (original[k + 1][0] == '\n'){
	continue;
     }
     token_count = 0;
     strcpy(line, original[k]);
     line_length = strlen(line);
     token = strtok(line, " ");
     while ( token != NULL ){
	token_count++;
	token = strtok(NULL, " ");
     }
     spaces = 1;
     spaces += (max_length - line_length) / (token_count - 1);
     extra_spaces = (max_length - line_length) % (token_count - 1);
     strcpy(line, original[k]);
     original[k][0] = '\0';
     token = strtok(line, " ");
     while ( token != NULL ){
	strcat(original[k], token);
	i = 0;
	while ( i < spaces ){
	   strcat(original[k], " ");
	   i ++;
	}
	if (extra_spaces){
	   extra_spaces--;
	   strcat(original[k], " ");
	}
	token = strtok(NULL, " ");
     }
  }
  free(line);
  free(token);
  return original;
}

// Pregatirea pentru paragraph. Avem un parametru obligatoriu si 2 optionali
// numere pozitive pe care le transformam cu char_to_int
// apelam paragraph. 
// returnam textul paragrafat daca avem o operatiune valida sau NULL in caz 
// contrar
char **prep_paragraph(char **original, char *operations, int original_line_count){
  int indent_length, start_line, end_line, k = 0; 
  char *indent_length_string ,*start_line_string, *end_line_string = NULL, 
       *token = (char *) calloc(MAX_CHAR_OP, sizeof(char));
  token = strtok(operations, " ");
  while ( token != NULL ){
     k++;
     if (k == 2){
	indent_length_string = token;
     }
     if (k == 3){
	start_line_string = token;
     }
     if (k == 4){
	end_line_string = token;
     }
     if (k > 4){
	printf("Invalid operation!\n");
	return NULL;
     }
     token = strtok(NULL, " ");
  }
  free(token);
  if (k == 1){
     printf("Invalid operation!\n");
     return NULL; 
  }
  else{
     indent_length = char_to_int(indent_length_string);
     if (indent_length == -1){
	printf("Invalid operation!\n");
	return NULL;
     }
     if (k >= 3){
	start_line = char_to_int(start_line_string);
	if (start_line == -1){
	   printf("Invalid operation!\n");
	   return NULL;
	}
	if (start_line > original_line_count){
	   return original;
	}
	if (k == 4){
	   end_line = char_to_int(end_line_string);
	   if (end_line == -1){
	      printf("Invalid operation!\n");
	      return NULL;
	   }
	}
	else{
	   end_line = original_line_count;
	}
	if (end_line < start_line){
	   printf("Invalid operation!\n");
           return NULL;
	}
	if (end_line >= original_line_count){
	   end_line = original_line_count;
	}
     }
     else{
	start_line = 0;
	end_line = original_line_count;
     }
  }
  paragraph(original, indent_length, start_line, end_line);
  return original;
}

// La fiecare linie din interval care este inceput de paragraf adaugam la 
// inceput n spatii
// Variabila ok este 1 daca linia curenta este inceput de paragraf si 0 altfel
// Daca ok este nenul, atunci adaugam spatii la inceputul liniei
void paragraph(char **original, int indent_length, int start_line, int end_line){
  int i, j, ok = 0;
  char *line = (char *) calloc(MAX_CHAR_LINE, sizeof(int));
  if (start_line == 0){
     ok = 1;
  }
  else{
     start_line --;
  }
  for (i = start_line; i <= end_line; i++){
     // daca urmeaza alt paragraf
     if (original[i][0] == '\n' ){
	ok = 1;
	continue;
     }
     if (ok){
	while ( original[i][0] == ' ' ){
	   original[i]++;
        }
	for (j = 0; j < indent_length; j++){
 	   strcpy(line, original[i]);
	   strcpy(original[i], " ");
	   strcat(original[i], line);
	}
	ok = 0;
     }
  }
  free(line);
}

// Pregatirea pentru liste. Avem 1 parametru obligatoriu si 2 optionali ( start
// si end_line ). 
// Aliniem toate liniile din interval la stanga. 
// Parametrul obligatoriu il trecem printr-un switch ca sa apelam funcita cores-
// punzatoare.
// Se returneaza NULL atunci cand original nu trebuie modificat deloc
// Se returneaza textul final in cazuri normale
char **prep_list(char **original, char *operations, int original_line_count){
  int start_line, end_line, k = 0;
  char *list_type, *special_char, *start_line_string, *end_line_string,
       *token = (char *) calloc(MAX_CHAR_OP, sizeof(char));
  token = strtok(operations, " ");
  while ( token != NULL ){
     k++;
     if (k == 2){
	list_type = token;
	if (strlen(list_type) > 1){
	   printf("Invalid operation!\n");
	   return NULL;
	}
     }
     if (k == 3){
	special_char = token;
     }
     if (k == 4){
	start_line_string = token;
     }
     if (k == 5){
	end_line_string = token;
     }
     if (k > 5){
	printf("Invalid operation!\n");
	return NULL;
     }
     token = strtok(NULL, " ");
  }
  free(token);
  if (k < 3){
     printf("Invalid operation!\n");
     return NULL;
  }
  if (k > 3){
     start_line = char_to_int(start_line_string);
     if (start_line == -1){
	printf("Invalid operation!\n");
	return NULL;
     }
     if (k == 5){
	end_line = char_to_int(end_line_string);
	if (end_line == -1){
	   printf("Invalid operation!\n");
	   return NULL;
	}
	if (start_line > end_line){
	   printf("Invalid operation!\n");
           return NULL;
	}
	if (end_line >= original_line_count){
	   end_line = original_line_count - 1;
	}
	if (start_line > original_line_count){
	   return original;
	}
     }
     else{
	end_line = original_line_count - 1;
     }
  }
  else{
     start_line = 0;
     end_line = original_line_count - 1;
  }
  align_left(original, start_line, end_line);
  switch (list_type[0]){
     case 'n':
	num_list(original, special_char, start_line, end_line);
	break;
     case 'a':
	alphabetic_list(original, list_type[0], special_char, start_line, 
		        end_line);
	break;
     case 'A':
	alphabetic_list(original, list_type[0], special_char, start_line, 
	   		end_line);
	break;
     case 'b':
	bullet_list(original, special_char, start_line, end_line);
	break;
     default:
	printf("Invalid operation!\n");
	return NULL;
	break;
  }
  return original;  
}

// Listele numerotate sunt realizate cu un sprintf pe fiecare linie pe care o 
// facem de forma [numar_de_ordine][special_char]_[continutul_liniei]
void num_list(char **original, char *special_char, int start_line, 
	      int end_line){
  int i, num;
  char *line = (char *) calloc(MAX_CHAR_LINE, sizeof(char));
  for (i = start_line, num = 1; i <= end_line; i++, num++){
     strcpy(line, original[i]);
     sprintf(original[i], "%d%s %s", num, special_char, line);
  }
}

// La fel ca la listele numerotate, insa in loc de un numar de ordine, avem o 
// litera de ordine. In codul ASCII literele consecutive au coduri consecutive,
// si de aceea cresc cu cate 1 valoarea variabilei counter de tip char 
void alphabetic_list(char **original, char counter, char *special_char, 
  		    int start_line, int end_line){
  int i;
  char *line = (char *) calloc(MAX_CHAR_LINE, sizeof(char));
  for (i = start_line; i <= end_line; i++, counter++){
     strcpy(line, original[i]);
     sprintf(original[i], "%c%s %s", counter, special_char, line);
  }
}

// La bullet_list adaugam la inceputul fiecarei linii doar caracterul special si
// un spatiu intre el si linia din original
void bullet_list(char **original, char *bullet, int start_line, int end_line){
  int i;
  char *line = (char *) calloc(MAX_CHAR_LINE, sizeof(char));
  for (i = start_line; i <= end_line; i++){
     strcpy(line, original[i]);
     sprintf(original[i], "%s %s", bullet, line);
  }
}

// Pregatirea pentru liste ordonate este aproape identica cu pregatirea pentru
// listele normale. Ceea ce difera este faptul ca ordonez liniile in ordinea
// ceruta inainte de apelarea acelorasi functii de liste
char **prep_olist(char **original, char *operations, int original_line_count){
  int start_line, end_line, k = 0;
  char *list_type, *special_char, *start_line_string, *end_line_string, *ordering,
       *token = (char *) calloc(MAX_CHAR_OP, sizeof(char));
  token = strtok(operations, " ");
  while ( token != NULL ){
     k++;
     if (k == 2){
	list_type = token;
	if (strlen(list_type) > 1){
	   printf("Invalid operation!\n");
	   return NULL;
	}
     }
     if (k == 3){
	special_char = token;
     }
     if (k == 4){
	ordering = token;
	if ((ordering[0] != 'a') && (ordering[0] != 'z')){
	   printf("Invalid operation!\n");
	   return NULL;
	}
     }
     if (k == 5){
	start_line_string = token;
     }
     if (k == 6){
	end_line_string = token;
     }
     if (k > 6){
	printf("Invalid operation!\n");
	return NULL;
     }
     token = strtok(NULL, " ");
  }
  free(token);
  if (k < 4){
     printf("Invalid operation!\n");
     return NULL;
  }
  if (k > 4){
     start_line = char_to_int(start_line_string);
     if (start_line == -1){
	printf("Invalid operation!\n");
	return NULL;
     }
     if (k == 6){
	end_line = char_to_int(end_line_string);
	if (end_line == -1){
	   printf("Invalid operation!\n");
	   return NULL;
	}
	if (start_line > end_line){
	   printf("Invalid operation!\n");
           return NULL;
	}
	if (end_line >= original_line_count){
	   end_line = original_line_count - 1;
	}
	if (start_line > original_line_count){
	   return original;
	}
     }
     else{
	end_line = original_line_count - 1;
     }
  }
  else{
     start_line = 0;
     end_line = original_line_count - 1;
  }
  order(original, start_line, end_line, ordering[0]);
  align_left(original, start_line, end_line);
  switch (list_type[0]){
     case 'n':
	num_list(original, special_char, start_line, end_line);
	break;
     case 'a':
	alphabetic_list(original, list_type[0], special_char, start_line, end_line);
	break;
     case 'A':
	alphabetic_list(original, list_type[0], special_char, start_line, end_line);
	break;
     case 'b':
	bullet_list(original, special_char, start_line, end_line);
	break;
     default:
	printf("Invalid operation!\n");
	return NULL;
	break;
  }
  return original;  
}

// Functia order se ocupa de ordonarea ascendenta sau descendenta a liniilor din
// original. Caracterul ordering decide regula de ordonare ( a sau z)
void order(char **original, int start_line, int end_line, char ordering){
  int i, j;
  char *line = (char *) calloc(MAX_CHAR_LINE, sizeof(char));
  if (ordering == 'a'){
     for (i = start_line; i < end_line; i++){
        for (j = i + 1; j <= end_line; j++){
	   if (strcmp(original[i], original[j]) > 0){
	      strcpy(line, original[i]);
	      strcpy(original[i], original[j]);
	      strcpy(original[j], line);
	   }
        }
     }
  }
  else{
     for (i = start_line; i < end_line; i++){
        for (j = i + 1; j <= end_line; j++){
	   if (strcmp(original[i], original[j]) < 0){
	      strcpy(line, original[i]);
	      strcpy(original[i], original[j]);
	      strcpy(original[j], line);
	   }
        }
     }
  }
  free(line);
}

// Functia rm_trailing_whitespace sterge trailing whitespace ul fiecarei linii
// din original
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
