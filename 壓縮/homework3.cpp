#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>

typedef struct htree {
	int value;
	unsigned char c;
	htree* left;
	htree* right;
}ht;

ht* construct_htree(ht ori255[]);
void wirte_treecondi(ht *t, FILE *w, unsigned char *buffer, int *time, unsigned char code[][256], unsigned char d_buffer[], int db_get);
void w_binary(FILE *w, unsigned char word, unsigned char *buffer, int *time);
void w_one_byte(FILE *w, unsigned char word, unsigned char *buffer,int *time);

int main() {
	FILE *f;
	printf("�n���Y�ɦW:");
	char name[20];
	
	gets(name);
	if (!(f = fopen(name, "rb+"))) {
		printf("File open failed.\n");
	}
	else {
		ht *tree, ASC[256] = { 0 };
		unsigned char by;

		int arrive=0,get=0;
		 fseek(f, 0L, 2);			//	get the size of the file
		 arrive = ftell(f);
		 fseek(f, 0L, 0);
		while (get<arrive) {		//�B�s�C��BYTE�X�{���W�v
			by = getc(f);
			ASC[by].value++;
			get++;
		}
		
		tree = construct_htree(ASC);
		
		if (tree == 0) {
			printf("�S����\n");
		}
		else {
			FILE *w;
			char namedat[20];
			int i=0;
			for(i=0;name[i]!='.';i++){	namedat[i]=name[i];	}
			namedat[i]='.';
			namedat[i+1]='d';
			namedat[i+2]='a';
			namedat[i+3]='t';
			namedat[i+4]='\0';
			if (!(w = fopen(namedat, "wb+"))) {
				printf("File create failed.\n");
			}
			else {
				fseek(f, 0L, 0);
				get = 0;
	
				unsigned char code[256][256] = {0},d_buffer[255]; //��V��BUFFER
				int db_get = 0;
	
				unsigned char buffer=0;
				int time = 0;
				double w_get=0;
				wirte_treecondi(tree, w,&buffer,&time,code,d_buffer,db_get);//�s�J�ڪ���	 
	
				while(name[i]!='\0'){
						w_one_byte(w,name[i+1],&buffer,&time);
						i++;
				}
	
				w_get=ftell(w);    //�s�������ɦW�ϥ�BYTE 
	
	
				while (get < arrive) {				//�s�J���I�ƾ�
					unsigned char fc;
					fc = getc(f);
					for (int i = 0; code[fc][i] != 255; i++) {
						w_binary(w, code[fc][i], &buffer, &time);
					}
					get++;
				}
			
				putc(buffer,w);				//��ѤU�Ӫ�BUFFER�������FILE 
				putc(char(time),w);
	
				printf("���Y����\n");
				double fbig=0,wbig=0;
			
				wbig=ftell(w);
				fbig=ftell(f);
				printf("�s����j�p : %d\n���ɮפj�p : %d\n�s�ɮפj�p : %d\n���Y�� : %f\n",int(w_get),int(fbig),int(wbig),wbig/fbig);
	
				fclose(w);
			}
		}
		fclose(f);
	}
	system("pause");
	return 0;
}

ht* construct_htree(ht ori[]) {
	ht *tree=0, *left, *right;
	while (true) {
		int first = -2, second = -1, fi = -1, si = -1,max=0,mi=-1;
		for (int i = 0; i <= 255; i++) {
			if (ori[i].value != 0 && (ori[i].value <= first || first == -2)) {
				first = ori[i].value;
				fi = i;
			}
			if (ori[i].value != 0 && (ori[i].value >= max)) {
				max = ori[i].value;
				mi = i;
			}
		}
	
		for (int i = 0; i <= 255; i++) {
			if (ori[i].value != 0 && (ori[i].value <= second || second == -1)&&i!=fi) {
				second = ori[i].value;
				si = i;
			}
		}
		if (first == -2) {			//�@�ӳ��S��
			return 0;
		}
		else if (second == -1) {	//�u�Ѥ@�ӤF
			if (tree != 0) {
				return tree;
			}
			else {
				tree = (ht *)malloc(sizeof(ht));
				tree->c = first;
				tree->left = 0; tree->right = 0;		//��l�Ʀۤv���p��
			}
		}
		else {											//��ӳ̤p���X�֦�TREE���FIRST���a�� SECOND�Q�R��
			if (ori[fi].c != 255 && ori[si].c != 255) {	//�o��ӳ����OTREE
				tree = (ht *)malloc(sizeof(ht));
				left = (ht *)malloc(sizeof(ht));
				right = (ht *)malloc(sizeof(ht));
				left->left = 0; left->right = 0;		//��l�ƥL�̪��p��
				right->left = 0; right->right = 0;		//��l�ƥL�̪��p��

				tree->left = left;						//�X���@��TREE
				tree->right = right;
				tree->value = first + second;
				tree->left->c = fi;
				tree->right->c = si;
	
				ori[fi].value = first + second;			//��Ʀr�X��
				ori[fi].c = 255;						//��ܳo��w�g�إߤFTREE
				ori[fi].left = tree;					//��b����
				ori[si].value = 0;						//���2�R��
			}
			else if ((ori[fi].c == 255 && ori[si].c != 255)) {	//�@�ӬOTREE
				tree = (ht *)malloc(sizeof(ht));
				right = (ht *)malloc(sizeof(ht));
				right->left = 0; right->right = 0;		//��l�ƥL�̪��p��
	
				tree->left = ori[fi].left;
				tree->right = right;
				tree->value = first + second;
				tree->right->c = si;
	
				ori[fi].value = first + second;			//��Ʀr�X��
				ori[fi].c = 255;						//��ܳo��w�g�إߤFTREE
				ori[fi].left = tree;					//��b����
				ori[si].value = 0;						//���2�R��
			}
			else if ((ori[fi].c != 255 && ori[si].c == 255)) {	//�@�ӬOTREE
				tree = (ht *)malloc(sizeof(ht));
				left = (ht *)malloc(sizeof(ht));
				left->left = 0; left->right = 0;		//��l�ƥL�̪��p��
	
				tree->left = left;
				tree->right = ori[si].left;
				tree->value = first + second;
				tree->left->c =fi;
	
				ori[fi].value = first + second;			//��Ʀr�X��
				ori[fi].c = 255;						//��ܳo��w�g�إߤFTREE
				ori[fi].left = tree;					//��b����
				ori[si].value = 0;						//���2�R��
			}
			else {										//��ӳ��OTREE
				tree = (ht *)malloc(sizeof(ht));
	
				tree->left = ori[fi].left;
				tree->right = ori[si].left;
				tree->value = first + second;
	
				ori[fi].value = first + second;			//��Ʀr�X��
				ori[fi].c = 255;						//��ܳo��w�g�إߤFTREE
				ori[fi].left = tree;					//��b����
				ori[si].value = 0;						//���2�R��
			}
		}
	}
}

void wirte_treecondi(ht *t,FILE *w, unsigned char *buffer, int *time, unsigned char code[][256], unsigned char d_buffer[],int db_get) {
	/*	��ت��p0=�U���OTREE 1=�ۤv���p�ĬONULL �ۤv�OCHAR 	*/
	if (t->left == NULL && t->right == NULL) {//1
		w_binary(w,1, buffer, time);
		
		for (int i = 0; i < db_get; i++) {
		code[t->c][i] = d_buffer[i];		//�⨺�ӼƦr����V��i�h	
		}code[t->c][db_get] = 255;			//�����аO

		//�g�U�ۤv��CHAR
		for (int i = 7; i >= 0; i--) {
			unsigned char c = t->c;
			c >>= i;
			c &= 1;
			w_binary(w,c, buffer, time);
		}
	}
	else {									//0
		w_binary(w,0, buffer, time);
		
		d_buffer[db_get] = 1;	//����VBUFFER�̭� ��ɭԥi�H�[
		db_get++;
		wirte_treecondi(t->right, w, buffer, time, code, d_buffer, db_get);

		db_get--;
		d_buffer[db_get] = 0;	//����VBUFFER�̭� ��ɭԥi�H�[
		db_get++;
		wirte_treecondi(t->left, w, buffer, time, code, d_buffer, db_get);
	}
}

void w_binary(FILE *w, unsigned char word, unsigned char *buffer,int *time) {
	if ((*time) == 8) {	//BUFFER���F�N�g��W�̭�
		fputc(*buffer, w);
		//printf("%d", *buffer);
		*buffer = 0;
		*time = 0;
	}
		//�g��BUFFER�̭�
		(*buffer) |= word << (7-*time);
		(*time)++;
}

void w_one_byte(FILE *w, unsigned char word, unsigned char *buffer,int *time){
	for(int j=7;j>=0;j--){
		unsigned tem=word;
		tem>>=j;
		tem&=1;
		w_binary(w,tem, buffer, time);
	}
	
}
