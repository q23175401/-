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
unsigned char ok=0;			//OK是BUFFER最後存進來 的 BUFFER 數量，放在檔尾

int main() {
	FILE *f;
	char name[20];
	printf("壓縮檔名:");

	gets(name);
	if (!(f = fopen(name, "rb+"))) {
		printf("File open failed.\n");
	}
	else {
		ht *tree;
		unsigned char by;

		fseek(f, -1L, 2);	//	get the size of the file
		ok = getc(f);		//拿到最後BUFFER可以到哪裡
		arrive = ftell(f);
		fseek(f, 0L, 0);

		unsigned char  buffer = 0;
		int b_count = 0;
		tree = recon_tree(f, &buffer, &b_count);		//重建我的樹

		//取回副檔名
		char namedat[20];
		int dot=0;
		for(;name[dot]!='.';dot++){	namedat[dot]=name[dot];	}
		namedat[dot]='.';dot++;	//放點點 	
		unsigned char d=BYTE_FORM_FILE(f,&buffer,&b_count);
		while(d!=0){
			namedat[dot]=d;
			dot++;
		 	d=BYTE_FORM_FILE(f,&buffer,&b_count);
		}namedat[dot]=d;
		printf("%s\n", namedat);
		
		if (tree == 0) {
			printf("沒有樹\n");
		}
		else {
			FILE *w;
			if (!(w = fopen(namedat, "wb+"))) {printf("File create failed.\n");}
			else {	//開重建的檔案
				
				unsigned char decode;//解出來的CHAR
				bool OK = 1;
				while (get = ftell(f)< arrive-1 && OK){		//直到
					OK = shfmtree(f, tree, &buffer, &b_count,&decode);
					if(OK==1) putc(decode, w);
				}					
				while(get = ftell(f)==arrive-1 && OK) {
					OK = shfmtree_last_buffer(f, w, tree, &buffer, &b_count);
				}
				printf("解壓縮完畢\n");
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
	if (condi == 0) {									//0是樹
		tree = (ht *)malloc(sizeof(ht));
		tree->right = recon_tree(f,buffer,b_count);		//先右邊
		tree->left = recon_tree(f, buffer, b_count);;	//再左邊
		return tree;
	}
	else {												//是1 是CHAR了
		tree = (ht *)malloc(sizeof(ht));
		
		unsigned char tem=0,c=0;
		for (int i = 7; i >= 0; i--) {					//讀一個BYTE_FORM_FILE
		onebit_fromf(f,buffer,b_count,&tem);
		c |= tem<<i;
		}

		tree->c = c;			//把C放入
		tree->left = NULL;		//變成NULL作為結束
		tree->right = NULL;		//變成NULL作為結束		
		return tree;
	}	
	return 0;
}

bool onebit_fromf(FILE *f,unsigned char * buffer,int *b_count, unsigned char *result) {	//如果要拿一個BIT出來時 COUNT為0 就從F讀取出來
	int getn = -1; bool OK=1;
		if (*b_count == 0) {															//當BUFFER的數量被用完
			*buffer = getc(f);															//讀FILE，ftell(f)會自動加1BYTE
			if (getn = ftell(f)==arrive) { OK=0; }										//如果讀到BUFFER最後的數量，數量不能輸出，就 不OK
			*b_count = 8;	//8表示裝滿
		}
		//輸出出來
		if (get == arrive - 1) {														//當我在處理最後一個BUFFER的時候
			if (*b_count - 1 < 8-ok) {													//最後一個BUFFER不可以輸出比ok(最後BUFFER允許輸出的數量)更多的BIT
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

unsigned char BYTE_FORM_FILE(FILE *f,unsigned char * buffer,int *b_count){				//讀一個BYTE_FORM_FILE
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
		if (tem == 1) {			//如果讀到1 就往樹的右邊走
			t = t->right;		
		}
		else {
			t = t->left;		//如果是0就往左走
		}
	}
	//走到底之後
	(*result) = t->c;
	return OK;
}

bool  shfmtree_last_buffer(FILE *f, FILE *w, ht *t, unsigned char * buffer, int *b_count) {
	bool outable = 1,put=0;
	while (outable==1 && put==0) {
		while (t->left != NULL && t->right != NULL) {
			if (*b_count == 8-ok) {				//最後BUFFER的COUNT用完但又沒走到底
				outable = 0;
				return 0;						//結束了
			}
			else {
				unsigned char tem = *buffer;	//READ ONE BIT FROM BUFFER
				(*b_count)--;
				tem >>= *b_count;
				tem &= 1;

				if   (tem == 1) {t = t->right;}	//如果讀到1 就往樹的右邊走
				else {t = t->left;			}	//如果是0就往左走
			}
		}
		if (outable == 1) {						//當我已經跑到底了還是可以輸出，我在輸出
			putc(t->c, w); 
			put = 1;							//表示放過了可以離開
		}
	}
	return outable;
}
