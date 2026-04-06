#include "contact.h"

void init_contact(contact *c)
{
    init_names(&(c->names));
    init_address(&(c->address));

    memset(&(c->workplace), 0, WORKPLACE_SIZE + 1);
    memset(&(c->position), 0, POSITION_SIZE + 1);
    memset(&(c->social_media_link), 0, SOCIAL_MEDIA_LINK_SIZE + 1);
    memset(&(c->phone_number), 0, PHONE_NUMBER_SIZE + 1);
    memset(&(c->email), 0, EMAIL_SIZE + 1);
}