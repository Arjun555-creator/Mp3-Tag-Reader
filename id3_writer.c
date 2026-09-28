#include <stdio.h> 
#include <string.h> 
#include "id3_writer.h"
#include "id3_reader.h" 
#include "id3_utils.h"
#include <stdlib.h>
int write_id3_tags(const char *filename, const TagData *data)
{
    FILE *file = fopen(filename, "rb");

    if (file == NULL)
    {
        return 1;
    }

    unsigned char header[10];

    fread(header, 1, 10, file);

    unsigned int old_tag_size =
        (header[6] << 21) |
        (header[7] << 14) |
        (header[8] << 7) |
        header[9];

    unsigned char *old_data = malloc(old_tag_size);

    if (old_data == NULL)
    {
        fclose(file);
        return 1;
    }

    fread(old_data, 1, old_tag_size, file);

    unsigned int new_tag_size = 0;
    unsigned int position = 0;

    while (position < old_tag_size)
    {
        char frame_id[5];

        memcpy(frame_id, old_data + position, 4);
        frame_id[4] = '\0';

        unsigned int frame_size =
            (old_data[position + 4] << 24) |
            (old_data[position + 5] << 16) |
            (old_data[position + 6] << 8) |
            old_data[position + 7];

        if (strcmp(frame_id, "TIT2") == 0)
        {
            new_tag_size += 10 + strlen(data->title) + 1;
        }
        else if (strcmp(frame_id, "TPE1") == 0)
        {
            new_tag_size += 10 + strlen(data->artist) + 1;
        }
        else if (strcmp(frame_id, "TALB") == 0)
        {
            new_tag_size += 10 + strlen(data->album) + 1;
        }
        else if (strcmp(frame_id, "TYER") == 0)
        {
            new_tag_size += 10 + strlen(data->year) + 1;
        }
        else if (strcmp(frame_id, "TCON") == 0)
        {
            new_tag_size += 10 + strlen(data->genre) + 1;
        }
        else if (strcmp(frame_id, "TXXX") == 0)
        {
            new_tag_size += 10 + strlen(data->comment) + 9;
        }
        else
        {
            new_tag_size += 10 + frame_size;
        }

        position += 10 + frame_size;
    }

    unsigned char *new_data = malloc(new_tag_size);

    if (new_data == NULL)
    {
        free(old_data);
        fclose(file);
        return 1;
    }

    position = 0;
    unsigned int new_position = 0;

    while (position < old_tag_size)
    {
        char frame_id[5];

        memcpy(frame_id, old_data + position, 4);
        frame_id[4] = '\0';

        unsigned int frame_size =
            (old_data[position + 4] << 24) |
            (old_data[position + 5] << 16) |
            (old_data[position + 6] << 8) |
            old_data[position + 7];

        char *value = NULL;

        if (strcmp(frame_id, "TIT2") == 0)
            value = data->title;
        else if (strcmp(frame_id, "TPE1") == 0)
            value = data->artist;
        else if (strcmp(frame_id, "TALB") == 0)
            value = data->album;
        else if (strcmp(frame_id, "TYER") == 0)
            value = data->year;
        else if (strcmp(frame_id, "TCON") == 0)
            value = data->genre;

        if (strcmp(frame_id, "TXXX") == 0)
        {
            unsigned int value_size = strlen(data->comment) + 9;

            memcpy(new_data + new_position, "TXXX", 4);

            new_data[new_position + 4] = (value_size >> 24) & 0xFF;
            new_data[new_position + 5] = (value_size >> 16) & 0xFF;
            new_data[new_position + 6] = (value_size >> 8) & 0xFF;
            new_data[new_position + 7] = value_size & 0xFF;

            memcpy(new_data + new_position + 8,
                   old_data + position + 8, 2);

            new_data[new_position + 10] = 0;

            memcpy(new_data + new_position + 11,
                   "comment", 7);

            new_data[new_position + 18] = 0;

            memcpy(new_data + new_position + 19,
                   data->comment,
                   strlen(data->comment));

            new_position += 10 + value_size;
        }
        else if (value != NULL)
        {
            unsigned int value_size = strlen(value) + 1;

            memcpy(new_data + new_position, frame_id, 4);

            new_data[new_position + 4] = (value_size >> 24) & 0xFF;
            new_data[new_position + 5] = (value_size >> 16) & 0xFF;
            new_data[new_position + 6] = (value_size >> 8) & 0xFF;
            new_data[new_position + 7] = value_size & 0xFF;

            memcpy(new_data + new_position + 8,
                   old_data + position + 8, 2);

            new_data[new_position + 10] = 0;

            memcpy(new_data + new_position + 11,
                   value,
                   strlen(value));

            new_position += 10 + value_size;
        }
        else
        {
            memcpy(new_data + new_position,
                   old_data + position,
                   10 + frame_size);

            new_position += 10 + frame_size;
        }

        position += 10 + frame_size;
    }

    fclose(file);

    file = fopen(filename, "wb");

    if (file == NULL)
    {
        free(old_data);
        free(new_data);
        return 1;
    }

    header[6] = (new_tag_size >> 21) & 0x7F;
    header[7] = (new_tag_size >> 14) & 0x7F;
    header[8] = (new_tag_size >> 7) & 0x7F;
    header[9] = new_tag_size & 0x7F;

    fwrite(header, 1, 10, file);
    fwrite(new_data, 1, new_tag_size, file);

    free(old_data);
    free(new_data);

    fclose(file);

    return 0;
}
int edit_tag(const char *filename, const char *tag, const char *value)
{
    TagData *data = read_id3_tags(filename);

    if (data == NULL)
    {
        return 1;
    }

    char *new_value = malloc(strlen(value) + 1);

    if (new_value == NULL)
    {
        free_tag_data(data);
        return 1;
    }

    strcpy(new_value, value);

    if (strcmp(tag, "-t") == 0)
    {
        free(data->title);
        data->title = new_value;
    }
    else if (strcmp(tag, "-a") == 0)
    {
        free(data->artist);
        data->artist = new_value;
    }
    else if (strcmp(tag, "-A") == 0)
    {
        free(data->album);
        data->album = new_value;
    }
    else if (strcmp(tag, "-y") == 0)
    {
        free(data->year);
        data->year = new_value;
    }
    else if (strcmp(tag, "-c") == 0)
    {
        free(data->comment);
        data->comment = new_value;
    }
    else if (strcmp(tag, "-g") == 0)
    {
        free(data->genre);
        data->genre = new_value;
    }
    else
    {
        free(new_value);
        free_tag_data(data);
        return 1;
    }

    if (write_id3_tags(filename, data) != 0)
    {
        free_tag_data(data);
        return 1;
    }

    free_tag_data(data);

    return 0;
}
