#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h> 

#define MAXLENGTH 512
#define MAXSIZE 256

void make_shell(){//
	system("clear");
	printf("************************\n");
	printf("*                      *\n");
	printf("*                      *\n");
	printf("*      Hosgeldin       *\n");
	printf("*                      *\n");
	printf("*                      *\n");
	printf("*                      *\n");
	printf("*                      *\n");
	printf("************************\n");
	sleep(1);
	system("clear");
}
void without_pipe_operation(char **splitted1){//pipe olmayan komutları gerçekleştirmek için kullanılan fonksiyon
	pid_t p1=fork();//process olusturulması
	if(p1 == -1){//process olusturulamadıysa 
		printf("Fork yapilamadi.\n"); 
        return; 
	}
	else if(p1 == 0){//child procces islemleri
		if(execvp(splitted1[0],splitted1) < 0){//eger girilen komut gecersizse
			printf("Gecersiz Komut.\n");
			return;//child processin isi biter.
		} 	
	}
	else{ 
		wait(NULL);//child process in bitmesini bekleme
		return; 
	}
}
int pipe_operation(char **splitted1,char **splitted2){//pipe olan komutları gerçekleştirmek için kullanılan fonksiyon
	int pipes[2];//pipe ların bilgisini tutmak için kullanılmıştır
  	
    if(pipe(pipes) == -1){//pipe lar oluşturuluyor
    	printf("Pipe olusturulamadi.\n");
        return 1;
    }
    pid_t pid = fork();//process olusturulması
    if(pid == -1){//process olusturulamadıysa 
       	printf("Fork yapilamadi.\n");
        return 1;
    } 
    if(pid == 0){//child process işlemleri
        close(pipes[1]);//pipe ın sonunu yazmaya kapatma
        dup2(pipes[0], 0);//pipe ı splitted2[0] dan splitted2 ye bağlama
        close(pipes[0]);//pipe ın sonunu okumaya kapatma
        if(execvp(splitted2[0], splitted2) < 0)//eger girilen komut gecersizse
        	printf("Gecersiz Komut1.\n");
        return 0;
    }
    else{//parent process işlemleri
        close(pipes[0]);//pipe ın sonunu yazmaya kapatma
        dup2(pipes[1], 1);//pipe ı splitted1[0] dan splitted1 e bağlama
        close(pipes[1]);//pipe ın sonunu okumaya kapatma
        if(execvp(splitted1[0], splitted1) < 0)//eger girilen komut gecersizse
        	printf("Gecersiz Komut1.\n");
        return 0;
   }
}
void operation(int flag,char **splitted1,char **splitted2){//gönderilen string dizileri ve flag e göre hangi operation ın yapılacağını belirler
	if(flag){//flag 1 ise pipe kullanılmıştır.
		pipe_operation(splitted1,splitted2);
	}
	else{//flag  ise pipe kullanılmamıştır.
		without_pipe_operation(splitted1);
	}
}
int split(const char ch,char *src,char **dest){//verilen karakter bilgisine göre src stringini destination string dizisine ayırır.
	int i;//iterasyon amaclı kullanılmıstır.
	int j=0;//word dizisi için iterasyon amaclı kullanılmıstır.
	int k=0;//destination string dizisi için iterasyon amaclı kullanılmıstır.
	int n=strlen(src);//verilen src stringinin uzunluk bilgisini tutar.
	int flag=0;//aranan karakterin olup olmadığının bilgisini tutar.
	char word[MAXLENGTH];//src stringini bölerken buffer amaclı kullanılmıştır.
	for(i=0;i<n;i++){//aranan karakterin olup olmadığının kontrolü
		if(src[i] == ch){
			flag=1;
		}
	}
	i=0;
	if(flag){//aranan karakter varsa src stringini bölerek dest string dizisine ekler.
		while(i < n){
			while(i < n && src[i] != ch){
				word[j++]=src[i];
				word[j]='\0';
				i++;
			}
			while(i < n && src[i] == ch)
				i++;
			
			if(strlen(word) != 0){
				dest[k]=(char *)malloc(strlen(word));
				strcpy(dest[k],word);
				k++;
				j=0;
				memset(word,'\0',MAXLENGTH);
			}
		} 
	}
	else{//aranan karakter yoksa src stringini dest string dizisine kopyalar.
		dest[0]=(char *)malloc(strlen(src));
		strcpy(dest[0],src);
	}
	return flag;
}
int processing(char *str,char **splitted1,char **splitted2){//verilen komutu işlemek için kullanılmıştır.
	int flag;//pipe olup olmadığının bilgisini tutar.
	char *buf[2];//pipe var ise bölünen kısımları geçici olarak tutmak için kullanılmıştır.

	flag=split('|',str,buf);//pipe a göre stringi ayırma
	if(flag){//pipe varsa
		split(' ',buf[0],splitted1);//splitted1 string dizisine ilk komutu boşluklara göre ayırır
		split(' ',buf[1],splitted2);//splitted2 string dizisine ikinci komutu boşluklara göre ayırır
	}
	else{//pipe yoksa
		split(' ',str,splitted1);////splitted1 string dizisine tek komutu boşluklara göre ayırır
	}
	return flag;//flag bilgisini döndürür
}
int main(){
	make_shell();//shellin acılıs ekranı
	char str[MAXLENGTH];//kullanıcıdan alınan komutu saklamak için kullanılmıştır.
	char dir[MAXLENGTH];//kullanıcının directory bilgisini saklamak için kullanılmıştır.
	char *splitted1[MAXLENGTH];//kullanıcıdan alınan komutun ayrılmış halini tutmak için kullanılmıştır.
	char *splitted2[MAXLENGTH];//eğer pipe varsa kullanıcıdan alınan diğer komutun ayrılmış halini tutar.
	int cycle=1;//exit gelene kadar kullanıcının programı kullanmasını sağlar
	int flag;//pipe olup olmadığının bilgisini saklar.
	
	
	while(cycle){
		getcwd(dir,sizeof(dir));//sistemden directory bilgisini alır.
		printf("%s:",dir);//kullanıcıya hangi directoryde olduğu bilgisi gösterilir.
		fgets(str, MAXLENGTH, stdin);//kullanıcıdan komut alınır.
		if(strlen(str) > 1){
			str[strlen(str)-1]='\0';//alınan komutdaki newline ı yok etme.
			if(!strcmp(str,"exit")){//eğer kullanıcı exit girdiyse
				printf("Gule gule.\n");
				cycle=0;//döngüden çıkılır.
			}
			else{//exit girmediyse
				flag=processing(str,splitted1,splitted2);//girdiği komut işlenir ve pipe olup olmadığı bilgisi alınır.
				operation(flag,splitted1,splitted2);//ayrılan komutlar ve pipe bilgisi işleme geçirilir.
			}
		}
	}
	return 0; 
}





















