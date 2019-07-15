//Header for TFT_SPI.c
/*VERSION 0.5
 - 10 numers 0..9 and 26 letters A..Z (high register only)
 - Low register letters are the same as HR (a = A, b = B, etc.)
 - Added ',' and '.'
 - 16.05.19 Added + - =
 */


//prototype
void Switch(char letter);	//function for choosing the number or letter array corresponding to 'letter' variable

//Array
char num_arr [20];	//array for number or letter

//20-element Arrays for numbers and letters 7x8 (indexes of pixels that have to be filled)
//Numbers and marks
char zero[sizeof(num_arr)]={2,3,4,8,12,15,18,19,22,24,26,29,30,33,36,40,44,45,46};	//array for 0
char uno[sizeof(num_arr)]={3,9,10,15,17,24,31,38,43,44,45,46,47};		//array for 1 
char two[sizeof(num_arr)]={2,3,4,8,12,19,25,31,37,43,44,45,46,47};	//2
char three[sizeof(num_arr)]={1,2,3,4,5,11,17,25,33,36,40,44,45,46};	//3
char four[sizeof(num_arr)]={4,10,11,16,18,22,25,29,30,31,32,33,39,46};	//4
char five[sizeof(num_arr)]={1,2,3,4,5,8,15,16,17,18,26,33,36,40,44,45,46};//5
char six[sizeof(num_arr)]={3,4,5,9,15,22,23,24,25,29,33,36,40,44,45,46};	//6
char seven[sizeof(num_arr)]={1,2,3,4,5,12,19,25,31,38,45};	//7
char eight[sizeof(num_arr)]={2,3,4,8,12,15,19,23,24,25,29,33,36,40,44,45,46};	//8
char nine[sizeof(num_arr)]={2,3,4,8,12,15,19,23,24,25,26,33,39,44,45};	//9
char point[sizeof(num_arr)]={36,37,43,44};		//.
char comma[sizeof(num_arr)]={36,37,43,44,50};	//,
char colon[sizeof(num_arr)] = {8,9,15,16,36,37,43,44};	//:
char underscore[sizeof(num_arr)] = {43,44,45,46,47}; // _
char minus [sizeof(num_arr)] = {29,30,31,32,33};	//-
char plus [sizeof(num_arr)] = {17,24,29,30,31,32,33,38,45};	//+
char equals [sizeof(num_arr)] = {22,23,24,25,26,36,37,38,39,40};	//=
//Letters
char A[sizeof(num_arr)]={2,3,4,8,12,15,19,22,26,29,30,31,32,33,36,40,43,47};	//'A'
char B[sizeof(num_arr)]={1,2,3,4,8,12,15,19,22,23,24,25,29,33,36,40,43,44,45,46};	//'B'
char C[sizeof(num_arr)]={2,3,4,8,12,15,22,29,36,40,44,45,46};	//'C'
char D[sizeof(num_arr)]={1,2,3,8,11,15,19,22,26,29,33,36,39,43,44,45};	//'D'
char E[sizeof(num_arr)]={1,2,3,4,5,8,15,22,23,24,25,29,36,43,44,45,46,47};	//'E'
char F[sizeof(num_arr)]={1,2,3,4,5,8,15,22,23,24,25,29,36,43};
char G[sizeof(num_arr)]={2,3,4,8,12,15,19,22,29,31,32,33,36,40,44,45,46,47};	//'G'
char H[sizeof(num_arr)]={1,5,8,12,15,19,22,23,24,25,26,29,33,36,40,43,47};	//'H'
char I[sizeof(num_arr)]={2,3,4,10,17,24,31,38,44,45,46};	//'I'
char J[sizeof(num_arr)]={3,4,5,11,18,25,32,36,39,44,45};	//'J'
char K[sizeof(num_arr)]={1,5,8,11,15,17,22,23,29,31,36,39,43,47};	//'K'
char L[sizeof(num_arr)]={1,8,15,22,29,36,43,44,45,46,47};	//'L'
char M[sizeof(num_arr)]={1,5,8,9,11,12,15,17,19,22,24,26,29,33,36,40,43,47};	//'M'
char N[sizeof(num_arr)]={1,5,8,12,15,16,19,22,24,26,29,32,33,36,40,43,47};	//'N'
char O[sizeof(num_arr)]={2,3,4,8,12,15,19,22,26,29,33,36,40,44,45,46};	//'O'
char P[sizeof(num_arr)]={1,2,3,4,8,12,15,19,22,23,24,25,29,36,43};	//'P'
char Q[sizeof(num_arr)]={2,3,4,8,12,15,19,22,26,29,31,33,36,39,44,45,47};	//'Q'
char R[sizeof(num_arr)]={1,2,3,4,8,12,15,19,22,23,24,25,29,31,36,39,43,47};	//'R'
char S[sizeof(num_arr)]={2,3,4,5,8,15,23,24,25,33,40,43,44,45,46};	//'S'
char T[sizeof(num_arr)]={1,2,3,4,5,10,17,24,31,38,45};	//'T'
char U[sizeof(num_arr)]={1,5,8,12,15,19,22,26,29,33,36,40,44,45,46};	//'U'
char V[sizeof(num_arr)]={1,5,8,12,15,19,22,26,29,33,37,39,45};	//'V'
char W[sizeof(num_arr)]={1,5,8,12,15,19,22,26,29,31,33,36,38,40,44,46};	//'W'
char X[sizeof(num_arr)]={1,5,8,12,16,18,24,30,32,36,40,43,47};	//'X'
char Y[sizeof(num_arr)]={1,5,8,12,16,18,24,31,38,45};		//'Y'
char Z[sizeof(num_arr)]={1,2,3,4,5,12,18,24,30,36,43,44,45,46,47};	//'Z'


//Function description
void Switch(char letter)
{
	if(letter=='a') letter = 'A';
	if(letter=='b') letter = 'B';
	if(letter=='c') letter = 'C';
	if(letter=='d') letter = 'D';
	if(letter=='e') letter = 'E';
	if(letter=='f') letter = 'F';
	if(letter=='g') letter = 'G';
	if(letter=='h') letter = 'H';
	if(letter=='i') letter = 'I';
	if(letter=='j') letter = 'J';
	if(letter=='k') letter = 'K';
	if(letter=='l') letter = 'L';
	if(letter=='m') letter = 'M';
	if(letter=='n') letter = 'N';
	if(letter=='o') letter = 'O';
	if(letter=='p') letter = 'P';
	if(letter=='q') letter = 'Q';
	if(letter=='r') letter = 'R';
	if(letter=='s') letter = 'S';
	if(letter=='t') letter = 'T';
	if(letter=='u') letter = 'U';
	if(letter=='v') letter = 'V';
	if(letter=='w') letter = 'W';
	if(letter=='x') letter = 'X';
	if(letter=='y') letter = 'Y';
	if(letter=='z') letter = 'Z';	
		switch (letter)
	{				//fill num_arr array with elements of chosen array
	case 0: for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = zero[i];
			break;	
	case 1: for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = uno[i];
			break;
	case 2: for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = two[i];
			break;
	case 3: for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = three[i];
			break;
	case 4: for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = four[i];
			break;	
	case 5: for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = five[i];
			break;
	case 6: for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = six[i];
			break;
	case 7: for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = seven[i];
			break;
	case 8: for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = eight[i];
			break;
	case 9: for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = nine[i];
			break;
	//////////////////////////////////////////////////////////////////
	case '0': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = zero[i];
			break;	
	case '1': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = uno[i];
			break;
	case '2': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = two[i];
			break;
	case '3': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = three[i];
			break;
	case '4': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = four[i];
			break;	
	case '5': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = five[i];
			break;
	case '6': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = six[i];
			break;
	case '7': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = seven[i];
			break;
	case '8': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = eight[i];
			break;
	case '9': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = nine[i];
			break;
	case 'A': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = A[i];
			break;
	case 'B': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = B[i];
			break;
	case 'C': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = C[i];
			break;
	case 'D': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = D[i];
			break;
	case 'E': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = E[i];
			break;
	case 'F': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = F[i];
			break;
	case 'G': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = G[i];
			break;
	case 'H': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = H[i];
			break;
	case 'I': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = I[i];
			break;
	case 'J': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = J[i];
			break;
	case 'K': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = K[i];
			break;
	case 'L': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = L[i];
			break;
	case 'M': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = M[i];
			break;
	case 'N': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = N[i];
			break;
	case 'O': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = O[i];
			break;
	case 'P': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = P[i];
			break;
	case 'Q': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = Q[i];
			break;
	case 'R': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = R[i];
			break;
	case 'S': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = S[i];
			break;
	case 'T': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = T[i];
			break;
	case 'U': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = U[i];
			break;
	case 'V': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = V[i];
			break;
	case 'W': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = W[i];
			break;
	case 'X': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = X[i];
			break;
	case 'Y': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = Y[i];
			break;
	case 'Z': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = Z[i];
			break;
	case '.': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = point[i];
			break;
	case ',': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = comma[i];
			break;
	case ':': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = colon[i];
			break;
	case '_': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = underscore[i];
			break;
	case '-': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = minus[i];
			break;
	case '+': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = plus[i];
			break;
	case '=': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = equals[i];
			break;		
	case ' ': for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = 0;	//Space 
			break;
	
	default: for (int i=0;i<sizeof(num_arr);i++) num_arr[i] = 0;	//nothing (space) default
	}//end switch
}//end Switch
