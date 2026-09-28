#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "id3_reader.h"
#include "error_handling.h"
TagData* read_id3_tags(const char *filename)
{
    FILE *file = fopen(filename, "rb");

    if (file == NULL)
    {
        return NULL;
    }

    char header[3];

    fread(header, 1, 3, file);

    if (header[0] != 'I' ||
        header[1] != 'D' ||
        header[2] != '3')
    {
        fclose(file);
        return NULL;
    }

    TagData *data = create_tag_data();

    if (data == NULL)
    {
        fclose(file);
        return NULL;
    }

    unsigned char major;
    unsigned char revision;
    unsigned char flags;
    unsigned char size[4];

    fread(&major, 1, 1, file);
    fread(&revision, 1, 1, file);
    fread(&flags, 1, 1, file);
    fread(size, 1, 4, file);

    unsigned int tag_size = (size[0] << 21) | (size[1] << 14) | (size[2] << 7) | size[3];
    unsigned int bytes_read = 0;

    if (major == 2)
    {
        while (bytes_read < tag_size)
        {
            char frame_id[4];
            fread(frame_id, 1, 3, file);
            frame_id[3] = '\0';
            unsigned char frame_size[3];
            fread(frame_size, 1, 3, file);
            unsigned int frame_data_size = (frame_size[0] << 16) | (frame_size[1] << 8) | frame_size[2];
            if (strcmp(frame_id, "TT2") == 0)
            {
                data->title = malloc(frame_data_size);

                if (data->title == NULL)
                {
                    fclose(file);
                    free_tag_data(data);
                    return NULL;
                }

                unsigned char encoding;

                fread(&encoding, 1, 1, file);
                fread(data->title, 1, frame_data_size - 1, file);

                data->title[frame_data_size - 1] = '\0';
            }
            else if (strcmp(frame_id, "TP1") == 0)
            {
                data->artist = malloc(frame_data_size);

                if (data->artist == NULL)
                {
                    fclose(file);
                    free_tag_data(data);
                    return NULL;
                }

                unsigned char encoding;

                fread(&encoding, 1, 1, file);
                fread(data->artist, 1, frame_data_size - 1, file);

                data->artist[frame_data_size - 1] = '\0';
            }
            else if (strcmp(frame_id, "TAL") == 0)
            {
                data->album = malloc(frame_data_size);

                if (data->album == NULL)
                {
                    fclose(file);
                    free_tag_data(data);
                    return NULL;
                }

                unsigned char encoding;

                fread(&encoding, 1, 1, file);
                fread(data->album, 1, frame_data_size - 1, file);

                data->album[frame_data_size - 1] = '\0';
            }
            else if (strcmp(frame_id, "TYE") == 0)
            {
                data->year = malloc(frame_data_size);

                if (data->year == NULL)
                {
                    fclose(file);
                    free_tag_data(data);
                    return NULL;
                }

                unsigned char encoding;

                fread(&encoding, 1, 1, file);
                fread(data->year, 1, frame_data_size - 1, file);

                data->year[frame_data_size - 1] = '\0';
            }
            else if (strcmp(frame_id, "TCO") == 0)
            {
                data->genre = malloc(frame_data_size);

                if (data->genre == NULL)
                {
                    fclose(file);
                    free_tag_data(data);
                    return NULL;
                }

                unsigned char encoding;

                fread(&encoding, 1, 1, file);
                fread(data->genre, 1, frame_data_size - 1, file);

                data->genre[frame_data_size - 1] = '\0';
            }
            else
            {
                fseek(file, frame_data_size, SEEK_CUR);
            }

            bytes_read += 6 + frame_data_size;
        }
    }
    else if (major == 3)
    {
        while (bytes_read < tag_size)
        {
            char frame_id[5];

            fread(frame_id, 1, 4, file);
            frame_id[4] = '\0';

            unsigned char frame_size[4];

            fread(frame_size, 1, 4, file);

            unsigned int frame_data_size = (frame_size[0] << 24) | (frame_size[1] << 16) | (frame_size[2] << 8) | frame_size[3];
            unsigned char frame_flags[2];
            fread(frame_flags, 1, 2, file);

            if (strcmp(frame_id, "TIT2") == 0)
            {
                data->title = malloc(frame_data_size);

                if (data->title == NULL)
                {
                    fclose(file);
                    free_tag_data(data);
                    return NULL;
                }

                unsigned char encoding;
                fread(&encoding, 1, 1, file);
                fread(data->title, 1, frame_data_size - 1, file);
                data->title[frame_data_size - 1] = '\0';
            }
            else if (strcmp(frame_id, "TPE1") == 0)
            {
                data->artist = malloc(frame_data_size);

                if (data->artist == NULL)
                {
                    fclose(file);
                    free_tag_data(data);
                    return NULL;
                }
                unsigned char encoding;
                fread(&encoding, 1, 1, file);
                fread(data->artist, 1, frame_data_size - 1, file);

                data->artist[frame_data_size - 1] = '\0';
            }
            else if (strcmp(frame_id, "TALB") == 0)
            {
                data->album = malloc(frame_data_size);

                if (data->album == NULL)
                {
                    fclose(file);
                    free_tag_data(data);
                    return NULL;
                }

                unsigned char encoding;

                fread(&encoding, 1, 1, file);
                fread(data->album, 1, frame_data_size - 1, file);

                data->album[frame_data_size - 1] = '\0';
            }
            else if (strcmp(frame_id, "TYER") == 0)
            {
                data->year = malloc(frame_data_size);

                if (data->year == NULL)
                {
                    fclose(file);
                    free_tag_data(data);
                    return NULL;
                }

                unsigned char encoding;
                fread(&encoding, 1, 1, file);
                fread(data->year, 1, frame_data_size - 1, file);
                data->year[frame_data_size - 1] = '\0';
            }
            else if (strcmp(frame_id, "TXXX") == 0)
            {
                data->comment = malloc(frame_data_size);

                if (data->comment == NULL)
                {
                    fclose(file);
                    free_tag_data(data);
                    return NULL;
                }

                unsigned char encoding;
                fread(&encoding, 1, 1, file);
                unsigned int i = 0;
                char ch;

                while (i < frame_data_size - 1)
                {
                    fread(&ch, 1, 1, file);

                    if (ch == '\0')
                    {
                        break;
                    }

                    i++;
                }

                int comment_size = frame_data_size - i - 2;
                fread(data->comment, 1, comment_size, file);
                data->comment[comment_size] = '\0';
            }
            else if (strcmp(frame_id, "COMM") == 0)
            {
                data->comment = malloc(frame_data_size);

                if (data->comment == NULL)
                {
                    fclose(file);
                    free_tag_data(data);
                    return NULL;
                }
                unsigned char encoding;
                fread(&encoding, 1, 1, file);
                fseek(file, 3, SEEK_CUR);
                int comment_size = frame_data_size - 4;
                fread(data->comment, 1, comment_size, file);
                data->comment[comment_size - 1] = '\0';
            }
            else if (strcmp(frame_id, "TCON") == 0)
            {
                data->genre = malloc(frame_data_size);

                if (data->genre == NULL)
                {
                    fclose(file);
                    free_tag_data(data);
                    return NULL;
                }

                unsigned char encoding;

                fread(&encoding, 1, 1, file);
                fread(data->genre, 1, frame_data_size - 1, file);
                data->genre[frame_data_size - 1] = '\0';
            }
            else
            {
                fseek(file, frame_data_size, SEEK_CUR);
            }

            bytes_read += 10 + frame_data_size;
        }
    }
    else
    {
        fclose(file);
        free_tag_data(data);
        return NULL;
    }

    data->version = malloc(10);

    if (data->version == NULL)
    {
        fclose(file);
        free_tag_data(data);
        return NULL;
    }

    sprintf(data->version, "ID3v2.%d", major);

    fclose(file);

    return data;
}
void display_metadata(const TagData *data)
{
    printf("Version : %s\n", data->version);

    if (data->title != NULL)
    {
        printf("Title : %s\n", data->title);
    }

    if (data->artist != NULL)
    {
        printf("Artist : %s\n", data->artist);
    }

    if (data->album != NULL)
    {
        printf("Album : %s\n", data->album);
    }
    if (data->year != NULL)
    {
        printf("Year : %s\n", data->year);
    }
    if (data->comment != NULL)
    {
        printf("Comment : %s\n", data->comment);
    }
    if (data->genre != NULL)
    {
        printf("Genre : %s\n", data->genre);
    }
}

void view_tags(const char *filename)
{
    TagData *data = read_id3_tags(filename);

    if (!data)
    {
        display_error("Failed to read ID3 tags.");
        return;
    }

    display_metadata(data);

    free_tag_data(data);
}
