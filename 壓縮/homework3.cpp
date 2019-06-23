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
	printf("要壓縮檔名:");
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
		while (get<arrive) {		//處存每種BYTE出現的頻率
			by = getc(f);
			ASC[by].value++;
			get++;
		}
		
		tree = construct_htree(ASC);
		
		if (tree == 0) {
			printf("沒有樹\n");
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
	
				unsigned char code[256][256] = {0},d_buffer[255]; //方向的BUFFER
				int db_get = 0;
	
				unsigned char buffer=0;
				int time = 0;
				double w_get=0;
				wirte_treecondi(tree, w,&buffer,&time,code,d_buffer,db_get);//存入我的樹	 
	
				while(name[i]!='\0'){
						w_one_byte(w,name[i+1],&buffer,&time);
						i++;
				}
	
				w_get=ftell(w);    //存完樹跟副檔名使用BYTE 
	
	
				while (get < arrive) {				//存入重點數據
					unsigned char fc;
					fc = getc(f);
					for (int i = 0; code[fc][i] != 255; i++) {
						w_binary(w, code[fc][i], &buffer, &time);
					}
					get++;
				}
			
				putc(buffer,w);				//把剩下來的BUFFER直接放到FILE 
				putc(char(time),w);
	
				printf("壓縮完畢\n");
				double fbig=0,wbig=0;
			
				wbig=ftell(w);
				fbig=ftell(f);
				printf("存取樹大小 : %d\n原檔案大小 : %d\n新檔案大小 : %d\n壓縮比 : %f\n",int(w_get),int(fbig),int(wbig),wbig/fbig);
	
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
		if (first == -2) {			//一個都沒有
			return 0;
		}
		else if (second == -1) {	//只剩一個了
			if (tree != 0) {
				return tree;
			}
			else {
				tree = (ht *)malloc(sizeof(ht));
				tree->c = first;
				tree->left = 0; tree->right = 0;		//初始化自己的小孩
			}
		}
		else {											//兩個最小的合併成TREE放到FIRST的地方 SECOND被刪掉
			if (ori[fi].c != 255 && ori[si].c != 255) {	//這兩個都不是TREE
				tree = (ht *)malloc(sizeof(ht));
				left = (ht *)malloc(sizeof(ht));
				right = (ht *)malloc(sizeof(ht));
				left->left = 0; left->right = 0;		//初始化他們的小孩
				right->left = 0; right->right = 0;		//初始化他們的小孩

				tree->left = left;						//合成一個TREE
				tree->right = right;
				tree->value = first + second;
				tree->left->c = fi;
				tree->right->c = si;
	
				ori[fi].value = first + second;			//把數字合併
				ori[fi].c = 255;						//表示這邊已經建立了TREE
				ori[fi].left = tree;					//放在左邊
				ori[si].value = 0;						//把第2刪除
			}
			else if ((ori[fi].c == 255 && ori[si].c != 255)) {	//一個是TREE
				tree = (ht *)malloc(sizeof(ht));
				right = (ht *)malloc(sizeof(ht));
				right->left = 0; right->right = 0;		//初始化他們的小孩
	
				tree->left = ori[fi].left;
				tree->right = right;
				tree->value = first + second;
				tree->right->c = si;
	
				ori[fi].value = first + second;			//把數字合併
				ori[fi].c = 255;						//表示這邊已經建立了TREE
				ori[fi].left = tree;					//放在左邊
				ori[si].value = 0;						//把第2刪除
			}
			else if ((ori[fi].c != 255 && ori[si].c == 255)) {	//一個是TREE
				tree = (ht *)malloc(sizeof(ht));
				left = (ht *)malloc(sizeof(ht));
				left->left = 0; left->right = 0;		//初始化他們的小孩
	
				tree->left = left;
				tree->right = ori[si].left;
				tree->value = first + second;
				tree->left->c =fi;
	
				ori[fi].value = first + second;			//把數字合併
				ori[fi].c = 255;						//表示這邊已經建立了TREE
				ori[fi].left = tree;					//放在左邊
				ori[si].value = 0;						//把第2刪除
			}
			else {										//兩個都是TREE
				tree = (ht *)malloc(sizeof(ht));
	
				tree->left = ori[fi].left;
				tree->right = ori[si].left;
				tree->value = first + second;
	
				ori[fi].value = first + second;			//把數字合併
				ori[fi].c = 255;						//表示這邊已經建立了TREE
				ori[fi].left = tree;					//放在左邊
				ori[si].value = 0;						//把第2刪除
			}
		}
	}
}

void wirte_treecondi(ht *t,FILE *w, unsigned char *buffer, int *time, unsigned char code[][256], unsigned char d_buffer[],int db_get) {
	/*	兩種狀況0=下面是TREE 1=自己的小孩是NULL 自己是CHAR 	*/
	if (t->left == NULL && t->right == NULL) {//1
		w_binary(w,1, buffer, time);
		
		for (int i = 0; i < db_get; i++) {
		code[t->c][i] = d_buffer[i];		//把那個數字的方向放進去	
		}code[t->c][db_get] = 255;			//結尾標記

		//寫下自己的CHAR
		for (int i = 7; i >= 0; i--) {
			unsigned char c = t->c;
			c >>= i;
			c &= 1;
			w_binary(w,c, buffer, time);
		}
	}
	else {									//0
		w_binary(w,0, buffer, time);
		
		d_buffer[db_get] = 1;	//放到方向BUFFER裡面 到時候可以加
		db_get++;
		wirte_treecondi(t->right, w, buffer, time, code, d_buffer, db_get);

		db_get--;
		d_buffer[db_get] = 0;	//放到方向BUFFER裡面 到時候可以加
		db_get++;
		wirte_treecondi(t->left, w, buffer, time, code, d_buffer, db_get);
	}
}

void w_binary(FILE *w, unsigned char word, unsigned char *buffer,int *time) {
	if ((*time) == 8) {	//BUFFER滿了就寫到W裡面
		fputc(*buffer, w);
		//printf("%d", *buffer);
		*buffer = 0;
		*time = 0;
	}
		//寫到BUFFER裡面
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
