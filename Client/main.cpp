#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <cstdlib>
#include <ctime>
#include<string>
#include<vector>
#include<iostream>
#include <arpa/inet.h>

using namespace std;

int gcd(int x,int y);
int isPrime(int p);
int power_mod(int number,int key, int n);
int encryption(int number,int key, int n);
int decryption(int number,int key, int n);
int modInverse(int a, int m);
int generateKey(int p);
int generatePrime();

int cards[52];
string suit[] = {"Hearts", "Club", "Diamond", "Spades"};
string facevalue[] = {"Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten", "Jack", "Queen", "King", "Ace"};
vector<string> deck;

int main()
{
   int money[1] = {1000},bettingMoney[1],mycards[5],cheating[1],win[1],tempo[52][2];
   int opponentMoney[1],opponentKey[1],opponentcards[5],opponentDecrycards[5],opponentPrime[1];
   int sockfd,n;
   struct sockaddr_in servaddr,cliaddr;
   socklen_t len;
   int i =0,j=0;
   int prime[1],key[1],inversekey[1];

   for(i=0;i<4;i++)
   {
       for(j=0;j<13;j++)
       {
           string s;
           s.append(facevalue[j]);
           s.append(" of ");
           s.append(suit[i]);
           deck.push_back(s);
       }
   }

   printf("This is bob the client");

   sockfd=socket(AF_INET,SOCK_DGRAM,0);
   bzero(&servaddr,sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr= inet_addr("127.0.0.1");
   servaddr.sin_port=htons(32002);
   len = sizeof(servaddr);

   while(money[0]>0)
    {
        bettingMoney[0] = 0;
        win[0] = 0;
        cheating[0] = 0;
        printf("\nMoney in hand : %d\n",money[0]);
        printf("\nGenerating Prime...");

        prime[0] = generatePrime();
        printf("\nGenerating Prime...[done]");

        sendto(sockfd,prime,sizeof(prime),0,(struct sockaddr *)&servaddr,len);
        printf("\nGenerating Key...");
        key[0] = generateKey(prime[0]-1);
        printf("\nGenerating Key...[done]");
        int temp[52];

        n = recvfrom(sockfd,temp,sizeof(temp),0,(struct sockaddr *)&servaddr,&len);

        // logic for selecting my and alice cards

        for(i = 0 ; i < 52 ;i++)
        {
            tempo[i][0]=temp[i];
            tempo[i][1] = 0;
        }
        i = 0;
        while(i < 10)
        {
            srand (time (NULL));
            int select = rand()%52;

            if(tempo[select][1] == 0)
            {

                if(i<5)
                {
                    opponentcards[i] = tempo[select][0];
                }
                else
                {
                    mycards[i-5] = tempo[select][0];
                }

                tempo[select][1] = 1;
                i++;
            }

        }

        for(int i = 0 ; i < 5 ; i++ )
        {
            mycards[i] = encryption(mycards[i],key[0],prime[0]);
        }

        sendto(sockfd,opponentcards,sizeof(opponentcards),0,(struct sockaddr *)&servaddr,len);
        sendto(sockfd,mycards,sizeof(mycards),0,(struct sockaddr *)&servaddr,len);
        n = recvfrom(sockfd,mycards,sizeof(mycards),0,(struct sockaddr *)&servaddr,&len);

        for(int i = 0 ; i < 5 ; i++ )
        {
            mycards[i] = decryption(mycards[i],modInverse(key[0],prime[0]-1),prime[0]);
        }

        for(int i = 0; i < 5 ;i++)
        {
            cout << "\nCard[" << i+1 <<"] : " << deck.at(mycards[i]-47);
            printf("\nAmount to bet for this card : $");
            int bet = 10000;
            while(bet > (money[0] - bettingMoney[0]))
            {
                scanf("%d",&bet);
            }
            bettingMoney[0] += bet;
        }
        printf("Total betting amount : %d",bettingMoney[0]);

        inversekey[0] = modInverse(key[0],prime[0]-1);

        n = recvfrom(sockfd,opponentMoney,sizeof(opponentMoney),0,(struct sockaddr *)&servaddr,&len);
        n = recvfrom(sockfd,opponentKey,sizeof(opponentKey),0,(struct sockaddr *)&servaddr,&len);
        n = recvfrom(sockfd,opponentDecrycards,sizeof(opponentDecrycards),0,(struct sockaddr *)&servaddr,&len);
        sendto(sockfd,bettingMoney,sizeof(bettingMoney),0,(struct sockaddr *)&servaddr,len);
        sendto(sockfd,mycards,sizeof(mycards),0,(struct sockaddr *)&servaddr,len);
        sendto(sockfd,inversekey,sizeof(inversekey),0,(struct sockaddr *)&servaddr,len);

        for(int i = 0;i<5 ; i++)
        {
            if(decryption(opponentcards[i],opponentKey[0],prime[0]) != opponentDecrycards[i])
            {
                cheating[0] = 1;
            }

        }
        if(cheating[0] == 1 )
        {
            printf("Opponent did cheating while playing by changing cards");
        }
        else
        {
            for(int i = 0; i<5 ;i++)
            {
                if((mycards[i]-47)%13 > (opponentDecrycards[i]-47)%13)
                {
                    win[0]++;
                }
                else if((mycards[i]-47)%13 < (opponentDecrycards[i]-47)%13)
                {
                    win[0]--;
                }
                else
                {
                    if(mycards[i] > opponentDecrycards[i])
                    {
                        win[0]++;
                    }
                    else
                    {
                        win[0]--;
                    }
                }
            }

            printf("Win = %d",win[0]);

            if(win[0] > 0)
            {
                money[0] += opponentMoney[0];
            }
            else
            {
                money[0] -= bettingMoney[0];
            }
        }

    }
}

int power_mod(int number,int key, int n)
{
    int i,y = 1;
    for(i = 0; i< key ; i++)
    {
        if(y > n)
        {
            y = y%n;
        }
        y = y*number;

    }
    return y%n;
}

int encryption(int number,int key, int n)
{
    return power_mod(number,key,n);
}

int decryption(int number,int key, int n)
{
    return power_mod(number,key,n);
}

int modInverse(int a, int m) {
    a %= m;
    for(int x = 1; x < m; x++)
    {
        if((a*x) % m == 1)
            return x;
    }
}

int generatePrime()
{
    srand ( time(NULL) );
    int prime;
    while(1)
    {
        prime = rand()%30000;
        if(prime > 20000 && prime < 30000)
        {
            if(isPrime(prime) == 1)
            {
                break;
            }
        }
    }

    return prime;

}

int isPrime(int p)
{
    for(int i = 2; i<p ; i++)
    {
        if(p%i == 0)
            return 0;
    }
    return 1;
}

int generateKey(int p)
{
    srand ( time(NULL) );
    int key;
    while(1)
    {
        key = rand()%p;
        if(key<p)
        {
            if(gcd(key,p)==1)
            {
                break;
            }
        }
    }
    return key;
}

int gcd(int x, int y)
{
    while(x!=y)
    {
        if(x>y)
            return gcd(x-y,y);
        else
            return gcd(x,y-x);
    }
    return x;
}
