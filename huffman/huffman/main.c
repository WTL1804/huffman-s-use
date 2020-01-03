//
//  main.c
//  huffman
//
//  Created by 王天亮 on 2019/12/31.
//  Copyright © 2019 王天亮. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
struct  numberOfchar {
    char s;
    int number;
    int tail;
};
struct Huffman {
    int weight;
    int parent, LChild, Rchild;
    char s;
};
struct codingString {
    char s[150];
    char chars;
};
struct compare {
    int min;
    int number;
};
void statistical(FILE *ftp, struct numberOfchar *array);
void coding(struct Huffman *huffman, int n, struct codingString *String);
void selected(struct Huffman *huffman, int n, int *s1, int *s2);
void creatHuffman(struct Huffman * huffman, int *arrayOfnumber, char *arrayOfChar, int n);
void compression(struct codingString *String, int *arrayOfnumber, char *arrayOfChar,int n, FILE *fp);
void unPack(FILE *ftp);
//统计字符个数
void statistical(FILE *ftp, struct numberOfchar *array) {
    FILE *fp;
    fp = ftp;
    //struct numberOfchar array[150] = {0};
    for (int i = 0; i < 150; i++) {
        (array+i)->number = 0;
        (array+i)->s = 0;
        (array+i)->tail = 0;
    }
    int tail = 0;
    while(!feof(fp)) {
        char ch = fgetc(fp);
        if (ch == EOF) {
            break;
        }
        int flag = 0;
        for (int i = 0; i < tail; i++) {
            if ((array+i)->s == ch) {
                flag++;
                (array+i)->number++;
                break;
            }
        }
        if (flag == 0) {
            (array+tail)->number++;
            (array+tail++)->s = ch;
        }
    }
    //实际数量为 tail - 1；
    (array+0)->tail = tail;
    //struct numberOfchar *p = array;
   // return p;
}
//设置编码
void coding(struct Huffman *huffman, int n, struct codingString *String) {
    for (int i = 0 ; i < 150; i++) {
        for (int j = 0; j < 150; j++) {
            (String + i)->s[j] = 0;
        }
    }
    for (int i = 0 ; i < n; i++) {
        int p = (huffman + i)->parent;
        int c = i;
        int flag = 0;
        (String + i)->chars = (huffman + i)->s;
        while(p != 0) {
            if ((huffman + p)->LChild == c) {
                (String + i)->s[flag++] = '0';
                c = p;
                p = (huffman + p)->parent;
            } else {
                (String + i)->s[flag++] = '1';
                c = p;
                p = (huffman +p)->parent;
            }
        }
        int tails = flag;
        for (int j = 0; j < flag/2; j++) {
            char temp = (String + i)->s[tails -1 ];
            (String + i)->s[tails - 1] = (String + i)->s[j];
            (String + i)->s[j] = temp;
        }

    }
}

//选择前n项中双亲为0且weight最小的俩个结点
void selected(struct Huffman *huffman, int n, int *s1, int *s2) {

    struct compare min, xmin;
    min.min = 99;
    xmin.min = 99;
    min.number = 99;
    xmin.number = 99;
    for (int i = 0; i < n; i++) {
        if ((huffman + i)->parent == 0) {
            if ((huffman + i)->weight < min.min) {
                min.number = i;
                min.min =(huffman + i)->weight;
            } else {
                continue;
            }
        } else {
            continue;
        }
    }
    for (int j = 0; j < n; j++) {
        if ((huffman + j)->parent == 0) {
            if ((xmin.min > (huffman + j)->weight) && j != min.number) {
               
                xmin.number = j;
                xmin.min = (huffman + j)->weight;
            }
        }
    }
    *s1 = min.number;
    *s2 = xmin.number;
}

//建立huffman树
void creatHuffman(struct Huffman * huffman, int *arrayOfnumber, char *arrayOfChar, int n) {
    int m = 2* n;
    for (int i = 0; i < n; i++) {
        (huffman + i)->LChild = 0;
        (huffman + i)->Rchild = 0;
        (huffman + i)->parent = 0;
        (huffman + i)->weight = arrayOfnumber[i];
        (huffman + i)->s = arrayOfChar[i];
    }
    for (int i = n; i < m-1; i++) {
        (huffman + i)->LChild = 0;
        (huffman + i)->Rchild = 0;
        (huffman + i)->parent = 0;
        (huffman + i)->weight = 0;
        (huffman + i)->s = 0;
    }
    for (int i = n; i < m -1 ; i++) {
        int s1 = 0, s2 =0;
        selected(huffman, i, &s1, &s2);
        
            (huffman + i)->weight = (huffman + s1)->weight +(huffman + s2)->weight;
            (huffman + i)->LChild = s1; (huffman + i)->Rchild = s2;
            (huffman + s1)->parent = i; (huffman + s2)->parent = i;
        
        //    (huffman + i)->weight = (huffman + s1)->weight;

        
    }
//    for (int i = 0; i < m - 1; i++) {
//        printf("%c 父节点：%d weight：%d 左孩子：%d 右孩子:%d\n",(huffman + i)->s, (huffman + i)->parent, (huffman + i)->weight, (huffman + i)->LChild, (huffman + i)->Rchild);
//    }
}

void compression(struct codingString *String, int *arrayOfnumber, char *arrayOfChar, int n ,FILE *fp) {
    FILE *ftp = fopen("/Users/wangtianliang/Desktop/test.txt", "wb+");
    if (ftp == NULL) {
        printf("创建压缩文件失败");
    }
    fwrite(&n, sizeof(int), 1, ftp);
    for (int i = 0; i < n; i++) {
        fwrite(&(arrayOfnumber[i]), sizeof(int), 1, ftp);
        fwrite(&(arrayOfChar[i]), sizeof(char), 1, ftp);
    }
    int t = 0;
    unsigned char temp = 0;
    while (!feof(fp)) {
        char tempChar = 0;
        fscanf(fp, "%c", &tempChar);
        for (int i = 0; i < n; i++) {
            if (tempChar == (String + i)->chars) {
                for (int j = 0; j < strlen((String + i)->s); j++) {
                    if (t == 8) {
                        fwrite(&temp, sizeof(char), 1, ftp);
                        t = 0;
                        temp = 0;
                    }
                    if ((String + i)->s[j] == '1') {
                        temp = (temp<<1)|1;
                    } else {
                        temp = temp<<1;
                    }
                    t++;
                }
            }
        }
    }
    while(t < 8 && t !=0) {
        temp = temp<<1;
        t++;
    }
    
    fwrite(&temp, sizeof(char), 1, ftp);
    fclose(ftp);
}
void unPack(FILE *ftp) {
    int n = 0;
    fread(&n, sizeof(int), 1, ftp);
    char arrayOfChar[150] = {0};
    int arrayOfNumber[150] = {0};
    int sumOFchar = 0;
    for (int i = 0 ; i < n; i++) {
        fread(&arrayOfNumber[i], sizeof(int), 1, ftp);
        fread(&arrayOfChar[i], sizeof(char), 1, ftp);
        sumOFchar = sumOFchar + arrayOfNumber[i];
    }
    struct Huffman unPackhuffman[150];
    creatHuffman(unPackhuffman, arrayOfNumber, arrayOfChar, n);
    struct codingString unPackString[150];
    coding(unPackhuffman, n, unPackString);
    unsigned char tempChar = 0;
    char unPackChar[150] = {0};
    int unPackCharOfTalis = 0;
    int n0 = 0;
    int number = 2 * n - 2;
    while(!feof(ftp)) {
        fread(&tempChar, sizeof(char), 1, ftp);
        //printf("%c", tempChar);
        
        for (int i = 7; i >= 0; i--) {
            n0 =(tempChar & 1<<i)!=0;
            if (n0 == 0) {
                number =  (unPackhuffman + number)->LChild;
            } else if (n0 == 1){
                number = (unPackhuffman + number)->Rchild;
            }
            if ((unPackhuffman + number)->LChild == 0 &&(unPackhuffman + number)->Rchild == 0) {
                unPackChar[unPackCharOfTalis++] = (unPackhuffman + number)->s;
                number = 2 * n - 2;
                printf("");
            }
            if (unPackCharOfTalis == sumOFchar) {
                break;
            }
        }
    }
    for (int i = 0; i < sumOFchar; i++) {
        printf("%c", unPackChar[i]);
    }
    FILE *upftp = fopen("/Users/wangtianliang/Desktop/testout.text", "w+");
    for (int i = 0; i < sumOFchar; i++) {
        fprintf(upftp, "%c", unPackChar[i]);
    }
    fclose(upftp);
}
int main(int argc, const char * argv[]) {
    // insert code here...
    FILE *fp;
    fp = fopen("/Users/wangtianliang/Desktop/c/baby.text", "r");
    if (fp == NULL) {
        printf("文件打开失败");
    }
    //array存放统计的字符个数/频度。
    struct numberOfchar array[150];
    statistical(fp, array);
    int arrayOfnumber[150] = {0};
    char arrayOfChar[150] = {0};
    for (int i = 0; i < array[0].tail; i++) {
        arrayOfnumber[i] = array[i].number;
        arrayOfChar[i] = array[i].s;
       printf("%c: %d\n", arrayOfChar[i], arrayOfnumber[i]);
    }
    
    struct Huffman huffman[150];
    creatHuffman(huffman, arrayOfnumber, arrayOfChar, array[0].tail);
    struct codingString String[150];
    coding(huffman, array[0].tail, String);
    for (int i = 0; i < array[0].tail; i++) {
        printf("%c:%s\n", String[i].chars, String[i].s);
    }
    FILE *ftp = fopen("/Users/wangtianliang/Desktop/test.txt", "wb+");
    if (ftp == NULL) {
        printf("创建压缩文件失败");
    }
    fclose(fp);
    fp = fopen("/Users/wangtianliang/Desktop/c/baby.text", "r");
    //实际个数为tail - 1;
    compression(String, arrayOfnumber, arrayOfChar, array[0].tail, fp);
   fclose(fp);
   // fclose(ftp);
    FILE * fdp = fopen("/Users/wangtianliang/Desktop/test.txt", "rb+");
    unPack(fdp);
    printf("\n解压完成\n");
    
    
    return 0;
}
