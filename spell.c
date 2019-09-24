#include "dictionary.h"
#include <string.h>
#include <stdlib.h>
#include <error.h>
#include <ctype.h>
#include <stdio.h>


node* create_node(char* word)
{
    if (strlen(word) > LENGTH) 
    {
        printf("Invalid word size\n");
        return NULL;
    }
    node *tmp = (node*)malloc(sizeof(node));
    strncpy(tmp->word, word, LENGTH);
    tmp->next = NULL;

    return tmp;
}

void make_lowercase(char* word)
{
    for(int i = 0; word[i]; i++)
    {
        word[i] = tolower(word[i]);
    }   
}

int check_words(FILE* fp, hashmap_t hashtable[], char* misspelled[])
{
    char *line = NULL;
    size_t len = 0;
    int mispelledCount = 0;
    /* Let's strip all this garbage */
    char delim[] = " .,!?;:()-”“*[]_1234567890<>@#$^&+={}|";

    while(getline(&line, &len, fp) != -1) 
    {
        /*strip CR and new line. seriously why does CR exist */
        line[strcspn(line, "\r\n")] = 0;
        char* word = strtok(line, delim); 
  
        while (word != NULL) 
        { 
            /* Let's settle for not checking words that start with apostrophes */
            if (word[0] != '\'')
            {
                char* word_to_check = strdup(word);
                /* if we don't find it, put it in our mispelled table. */
                if (!check_word(word_to_check, hashtable)) 
                {
                    misspelled[mispelledCount++] = word_to_check;
                    if (mispelledCount > MAX_MISSPELLED - 1) break;
                }
                else
                {
                    /* discard correct word */
                    free(word_to_check);
                }
            }
          
            word = strtok(NULL, delim); 
        } 
        /* lets not exceed our misspelled array */
        if (mispelledCount > MAX_MISSPELLED - 1) break;

    }
    free(line);

    return mispelledCount;
}

bool load_dictionary(const char* dictionary_file, hashmap_t hashtable[])
{
    /* start hashtable indices at NULL */
    for (int i = 0; i < HASH_SIZE; i++) hashtable[i] = NULL;

    FILE *fp = fopen(dictionary_file, "r");
    if(fp == NULL) 
    {
          perror("Unable to open file!");
          exit(1);
    }
 
    char* word = NULL;
    size_t len = 0;

    while(getline(&word, &len, fp) != -1) 
    {
        /* Strip new lines*/
        word[strcspn(word, "\r\n")] = 0;
        char* parsed_word = strdup(word);
        
        if (parsed_word != NULL)
        {
            /* make it lower case and then create the node */
            make_lowercase(parsed_word);
            node* new_word = create_node(parsed_word);

            /* If weve hit that bucket before, insert into LL */
            if (hashtable[hash_function(parsed_word)] != NULL)
            {
                node* last = hashtable[hash_function(parsed_word)];
                while (last->next != NULL)
                {
                    last = last->next;
                }
                last->next = new_word;
            }
            else
            {
                /* or lets init that bucket */
                hashtable[hash_function(parsed_word)] = new_word;
            }

        }

        free(parsed_word);

    }

    fclose(fp);
    free(word); 

    return true;

}

bool check_word(const char* word, hashmap_t hashtable[])
{
    /* lowercase word for matching */
    char* lower_word = strdup(word);
    make_lowercase(lower_word);

    /* get bucket */
    node* check_node = hashtable[hash_function(lower_word)];
    /* if bucket is empty, word doesn't exist! It's really that easy folks. */
    if (check_node == NULL)
    {
        free(lower_word);
        return false;
    }
    /* If not, lets check the linked list */
    while (1)
    {
        if (!strncmp(check_node->word, lower_word, strlen(check_node->word))) 
        {
            free(lower_word);
            return true;
        }
        if (check_node->next == NULL) break;
        check_node = check_node->next;
    }

    free(lower_word);
    /* Alright, couldn't find it */
    return false;
}

