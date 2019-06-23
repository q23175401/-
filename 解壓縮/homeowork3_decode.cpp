#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>

typedef struct htree {
		int value;
	unsigned char c;
	htree* left;
	htree* right;
}ht;

ht *recon_tree(FILE *f, unsigned char * buffer, int *b_count);
bool onebit_fromf(FILE *f, unsigned char * buffer, int *b_count, unsigned char *result);
bool  shfmtree(FILE *f,ht *t, unsigned char * buffer, int *b_count, unsigned char *result);
bool  shfmtree_last_buffer(FILE *f, FILE *w, ht *t, unsigned char * buffer, int *b_count);
unsigned char BYTE_FORM_FILE(FILE *f,unsigned char * buffer,int *b_count);

int arrive = 0, get = 0;
unsigned char ok=0;			//OK�OBUFFER�̫�s�i�� �� BUFFER �ƶq�A��b�ɧ�

int main() {
	FILE *f;
	char name[20];
	printf("���Y�ɦW:");

	gets(name);
	if (!(f = fopen(name, "rb+"))) {
		printf("File open failed.\n");
	}
	else {
		ht *tree;
		unsigned char by;

		fseek(f, -1L, 2);	//	get the size of the file
		ok = getc(f);		//����̫�BUFFER�i�H�����
		arrive = ftell(f);
		fseek(f, 0L, 0);

		unsigned char  buffer = 0;
		int b_count = 0;
		tree = recon_tree(f, &buffer, &b_count);		//���اڪ���

		//���^���ɦW
		char namedat[20];
		int dot=0;
		for(;name[dot]!='.';dot++){	namedat[dot]=name[dot];	}
		namedat[dot]='.';dot++;	//���I�I 	
		unsigned char d=BYTE_FORM_FILE(f,&buffer,&b_count);
		while(d!=0){
			namedat[dot]=d;
			dot++;
		 	d=BYTE_FORM_FILE(f,&buffer,&b_count);
		}namedat[dot]=d;
		printf("%s\n", namedat);
		
		if (tree == 0) {
			printf("�S����\n");
		}
		else {
			FILE *w;
			if (!(w = fopen(namedat, "wb+"))) {printf("File create failed.\n");}
			else {	//�}���ت��ɮ�
				
				unsigned char decode;//�ѥX�Ӫ�CHAR
				bool OK = 1;
				while (get = ftell(f)< arrive-1 && OK){		//����
					OK = shfmtree(f, tree, &buffer, &b_count,&decode);
					if(OK==1) putc(decode, w);
				}					
				while(get = ftell(f)==arrive-1 && OK) {
					OK = shfmtree_last_buffer(f, w, tree, &buffer, &b_count);
				}
				printf("�����Y����\n");
				fclose(w);
			}
		}	
		fclose(f);
	}
	system("pause");
	return 0;
}

ht *recon_tree(FILE *f, unsigned char * buffer, int *b_count) {
	unsigned char condi = 2;
	ht *tree;

	onebit_fromf(f,buffer,b_count,&condi);		
	if (condi == 0) {									//0�O��
		tree = (ht *)malloc(sizeof(ht));
		tree->right = recon_tree(f,buffer,b_count);		//���k��
		tree->left = recon_tree(f, buffer, b_count);;	//�A����
		return tree;
	}
	else {												//�O1 �OCHAR�F
		tree = (ht *)malloc(sizeof(ht));
		
		unsigned char tem=0,c=0;
		for (int i = 7; i >= 0; i--) {					//Ū�@��BYTE_FORM_FILE
		onebit_fromf(f,buffer,b_count,&tem);
		c |= tem<<i;
		}

		tree->c = c;			//��C��J
		tree->left = NULL;		//�ܦ�NULL�@������
		tree->right = NULL;		//�ܦ�NULL�@������		
		return tree;
	}	
	return 0;
}

bool onebit_fromf(FILE *f,unsigned char * buffer,int *b_count, unsigned char *result) {	//�p�G�n���@��BIT�X�Ӯ� COUNT��0 �N�qFŪ���X��
	int getn = -1; bool OK=1;
		if (*b_count == 0) {															//��BUFFER���ƶq�Q�Χ�
			*buffer = getc(f);															//ŪFILE�Aftell(f)�|�۰ʥ[1BYTE
			if (getn = ftell(f)==arrive) { OK=0; }										//�p�GŪ��BUFFER�̫᪺�ƶq�A�ƶq�����X�A�N ��OK
			*b_count = 8;	//8��ܸ˺�
		}
		//��X�X��
		if (get == arrive - 1) {														//��ڦb�B�z�̫�@��BUFFER���ɭ�
			if (*b_count - 1 < 8-ok) {													//�̫�@��BUFFER���i�H��X��ok(�̫�BUFFER���\��X���ƶq)��h��BIT
				OK = 0;
			}
		}
		unsigned char tem = *buffer;
		(*b_count)--;
		tem >>= *b_count;
		tem &= 1;
		(*result) = tem;
		return OK;
}

unsigned char BYTE_FORM_FILE(FILE *f,unsigned char * buffer,int *b_count){				//Ū�@��BYTE_FORM_FILE
		unsigned char tem=0,c=0;
		for (int i = 7; i >=0; i--) {	
		onebit_fromf(f,buffer,b_count,&tem);
		c |= tem<<i;
		}
		return c;
}

bool  shfmtree(FILE *f,ht *t, unsigned char * buffer, int *b_count, unsigned char *result) {
	unsigned char tem=0;
	bool OK=1;

	while(t->left != NULL && t->right != NULL) {
		OK=onebit_fromf(f, buffer, b_count,&tem);
		if (OK == 0) break;
		if (tem == 1) {			//�p�GŪ��1 �N���𪺥k�䨫
			t = t->right;		
		}
		else {
			t = t->left;		//�p�G�O0�N������
		}
	}
	//���쩳����
	(*result) = t->c;
	return OK;
}

bool  shfmtree_last_buffer(FILE *f, FILE *w, ht *t, unsigned char * buffer, int *b_count) {
	bool outable = 1,put=0;
	while (outable==1 && put==0) {
		while (t->left != NULL && t->right != NULL) {
			if (*b_count == 8-ok) {				//�̫�BUFFER��COUNT�Χ����S�S���쩳
				outable = 0;
				return 0;						//�����F
			}
			else {
				unsigned char tem = *buffer;	//READ ONE BIT FROM BUFFER
				(*b_count)--;
				tem >>= *b_count;
				tem &= 1;

				if   (tem == 1) {t = t->right;}	//�p�GŪ��1 �N���𪺥k�䨫
				else {t = t->left;			}	//�p�G�O0�N������
			}
		}
		if (outable == 1) {						//��ڤw�g�]�쩳�F�٬O�i�H��X�A�ڦb��X
			putc(t->c, w); 
			put = 1;							//��ܩ�L�F�i�H���}
		}
	}
	return outable;
}
