#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qrcode.h"
#include <windows.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <winsock.h>
#include <MYSQL/mysql.h>
#include <gtk/gtk.h>

GtkWidget *lastName,
        *lastNameForm,
        *lastNameSize,

        *firstName,
        *firstNameForm,
        *firstNameSize,

        *address,
        *addressForm,
        *addressSize,

        *mail,
        *mailForm,
        *mailSize,

        *job,
        *jobForm,
        *jobSize,

        *phoneNumber,
        *phoneNumberForm,
        *phoneNumberSize,

        *hbox, *button, *entry;

GtkBuilder *builder;
GtkWidget *window;

const char *receiveFirstName,
        *receiveLastName,
        *receiveEmail,
        *receiveAddress,
        *receiveJob,
        *receivePhoneNumber;

const char *path = "..\\decode_qrcode\\qr_code_1.png";

//GTK Entry Function

static bool lastName_clicked(GtkWidget *widget, GtkWidget *entry) {

    receiveLastName = gtk_entry_get_text(GTK_ENTRY(entry));
    lastNameSize = gtk_entry_get_text_length (GTK_ENTRY(entry));
    if( lastNameSize == 0 ){
        return false;
    }else{
        g_print("%s\n", receiveLastName);
        return true;
    }

}

static bool firstName_clicked(GtkWidget *widget, GtkWidget *entry) {

    receiveFirstName = gtk_entry_get_text(GTK_ENTRY(entry));
    firstNameSize = gtk_entry_get_text_length(GTK_ENTRY(entry));
    if( firstNameSize == 0 ){
        return false;
    }else{
        g_print("%s\n", receiveFirstName);
        return true;
    }

}

static bool address_clicked(GtkWidget *widget, GtkWidget *entry) {

    receiveAddress = gtk_entry_get_text(GTK_ENTRY(entry));
    addressSize = gtk_entry_get_text_length(GTK_ENTRY(entry));
    if( addressSize == 0 ){
        return false;
    }else{
        g_print("%s\n", receiveAddress);
        return true;
    }

}

static void email_clicked(GtkWidget *widget, GtkWidget *entry) {

    receiveEmail = gtk_entry_get_text(GTK_ENTRY(entry));
    mailSize = gtk_entry_get_text_length(GTK_ENTRY(entry));
    if( strchr(receiveEmail,'@') == NULL || mailSize == 0){
        gtk_entry_set_text(GTK_ENTRY(entry),"Email non valide");
        return false;
    }else{
        g_print("%s\n", receiveEmail);
        return true;
    }

}

static void job_clicked(GtkWidget *widget, GtkWidget *entry) {

    receiveJob = gtk_entry_get_text(GTK_ENTRY(entry));
    jobSize = gtk_entry_get_text_length(GTK_ENTRY(entry));
    if( jobSize == 0 ){
        return false;
    }else{
        g_print("%s\n", receiveJob);
        return true;
    }

}

static void phoneNumber_clicked(GtkWidget *widget, GtkWidget *entry) {

    receivePhoneNumber = gtk_entry_get_text(GTK_ENTRY(entry));
    phoneNumberSize = gtk_entry_get_text_length(GTK_ENTRY(entry));
    if( phoneNumberSize == 0 ){
        return false;
    }else{
        g_print("%s\n", receivePhoneNumber);
        return true;
    }

}


//Insert on DATABASE

void finish_with_error(MYSQL *con) {

    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
}

void add(GtkWidget *widget, GtkWidget *window) {
    MYSQL *con = mysql_init(NULL); //NULL = alocates / initializes and returns a new object

    if (con == NULL) {
        fprintf(stderr, "%s\n", mysql_error(con));
    }


    if (mysql_real_connect(con, "localhost", "tedanvi", "kLKLxEe8M1EfOdvG", "concierge_expert", 0, NULL, 0) == NULL) {
        finish_with_error(con);
    }

    //verif mail exist
    char testMail[] = "SELECT id FROM serviceprovider WHERE mail = '";
    strcat(testMail, receiveEmail);
    strcat(testMail, "'");

    if (mysql_query(con, testMail)) {
        finish_with_error(con);
    }

    MYSQL_RES *result = mysql_store_result(con);

    if (result == NULL) {
        finish_with_error(con);
    }

    int num_rows = mysql_num_rows(result);

    if (num_rows == 0) {

        int8_t reqInsertJob[255] = "INSERT INTO job (name) VALUES('";
        strcat(reqInsertJob, receiveJob);
        strcat(reqInsertJob, "')");

        if (mysql_query(con, reqInsertJob)) {
            finish_with_error(con);
        }

        char testJob[] = "SELECT id FROM job WHERE name = '";
        strcat(testJob, receiveJob);
        strcat(testJob, "'");

        if (mysql_query(con, testJob)) {
            finish_with_error(con);
        }

        MYSQL_RES *resultJob = mysql_store_result(con);

        if (resultJob == NULL) {
            finish_with_error(con);
        }

        MYSQL_ROW rowJob = mysql_fetch_row(resultJob);

        int8_t reqInsert[700] = "INSERT INTO serviceprovider (firstName, lastName, mail, tel, qrcode,idJob) VALUES('";
        strcat(reqInsert, receiveFirstName);
        strcat(reqInsert, "','");
        strcat(reqInsert, receiveLastName);
        strcat(reqInsert, "','");
        strcat(reqInsert, receiveEmail);
        strcat(reqInsert, "','");
        strcat(reqInsert, receivePhoneNumber);
        strcat(reqInsert, "','");
        strcat(reqInsert, path);
        strcat(reqInsert, "','");
        strcat(reqInsert, rowJob[0]);
        strcat(reqInsert, "')");

        if (mysql_query(con, reqInsert)) {
            finish_with_error(con);
        }

    } else {

        printf("The email is already used");
    }

    mysql_free_result(result);
    mysql_close(con);
}

//Verif Insert

void verif(GtkWidget *widget, GtkWidget *window) {

    MYSQL *con = mysql_init(NULL);

    if (con == NULL) {
        fprintf(stderr, "%s\n", mysql_error(con));
    }

    if (mysql_real_connect(con, "localhost", "tedanvi", "kLKLxEe8M1EfOdvG", "concierge_expert", 0, NULL, 0) == NULL) {
        finish_with_error(con);
    }

    char testCon[500] = "SELECT id FROM serviceprovider WHERE mail = '";
    strcat(testCon, receiveEmail);
    strcat(testCon, "' AND tel = '");
    strcat(testCon, receivePhoneNumber);
    strcat(testCon, "'");

    if (mysql_query(con, testCon)) {
        finish_with_error(con);
    }

    MYSQL_RES *result = mysql_store_result(con);

    if (result == NULL) {
        finish_with_error(con);
    }

    int num_rows = mysql_num_rows(result);

    if (num_rows == 0) {

        printf("Email or Phone Number invalid");

    } else {
        printf("ok");
        //exit(1);
    }

    mysql_free_result(result);
    mysql_close(con);
}

bool verifFileds(){
    if( lastName_clicked && firstName_clicked && address_clicked && email_clicked && job_clicked && phoneNumber_clicked ){
        return true;
    }else{
        return false;
    }
}

//Input GTK Form
void form(){
    GError *err = NULL;

    /* Creation d'un nouveau GtkBuilder */
    builder = gtk_builder_new();

    if( builder != NULL){
        gtk_builder_add_from_file(builder,"provider.glade",&err);

        gtk_builder_connect_signals(builder, NULL);

        window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));

        gtk_widget_show(window);

        lastName = GTK_WIDGET(gtk_builder_get_object(builder, "lastName"));
        lastNameForm = GTK_WIDGET(gtk_builder_get_object(builder, "lastNameForm"));

        firstName = GTK_WIDGET(gtk_builder_get_object(builder, "firstName"));
        firstNameForm = GTK_WIDGET(gtk_builder_get_object(builder, "firstNameForm"));

        address = GTK_WIDGET(gtk_builder_get_object(builder, "address"));
        addressForm = GTK_WIDGET(gtk_builder_get_object(builder, "addressForm"));

        mail = GTK_WIDGET(gtk_builder_get_object(builder, "mail"));
        mailForm = GTK_WIDGET(gtk_builder_get_object(builder, "mailForm"));

        job = GTK_WIDGET(gtk_builder_get_object(builder, "job"));
        jobForm = GTK_WIDGET(gtk_builder_get_object(builder, "jobForm"));

        phoneNumber = GTK_WIDGET(gtk_builder_get_object(builder, "phoneNumber"));
        phoneNumberForm = GTK_WIDGET(gtk_builder_get_object(builder, "phoneNumberForm"));

        button = GTK_WIDGET(gtk_builder_get_object(builder, "button"));

        //To send the variable entry
        g_signal_connect(button, "clicked", G_CALLBACK(lastName_clicked), lastNameForm);
        g_signal_connect(button, "clicked", G_CALLBACK(firstName_clicked), firstNameForm);
        g_signal_connect(button, "clicked", G_CALLBACK(address_clicked), addressForm);
        g_signal_connect(button, "clicked", G_CALLBACK(email_clicked), mailForm);
        g_signal_connect(button, "clicked", G_CALLBACK(job_clicked), jobForm);
        g_signal_connect(button, "clicked", G_CALLBACK(phoneNumber_clicked), phoneNumberForm);
        g_signal_connect(button, "clicked", G_CALLBACK(add),window);

        g_signal_connect(button, "clicked", G_CALLBACK(gtk_main_quit), NULL);

        g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);

        gtk_widget_show_all(window);

        gtk_main();
    }else{
        /* Affichage du message d'erreur de GTK+ */
         g_error("%s", err->message);
         g_error_free (err);
    }
}

static void doBasicDemo(void);
static void printQr(const uint8_t qrcode[]);
void SDL_ExitWithError(const char *message);

int main(int argc, char **argv) {
    gtk_init(&argc, &argv);
    form();
    doBasicDemo();
    return EXIT_SUCCESS;
}

static void doBasicDemo(void) {

    char metier[100]="Job:";
    char mail[100]="Mail:";
    char test[255] =" ";
    char test2[255]=" ";

    //For QR_Code concat GTK-Entry variable for Job and Phone Number Export
   char *concat = strcat(test,strcat(metier,receiveJob));
   char *step = strcat(mail,receiveEmail);
   char *step2 = strcat(step,concat);
   char *step3= strcat(test2,step2);

   const char *text =step2;

    enum qrcodegen_Ecc errCorLvl = qrcodegen_Ecc_HIGH;  // Error correction level

    // Make and print the QR Code symbol
    uint8_t qrcode[qrcodegen_BUFFER_LEN_MAX];
    uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];
    bool ok = qrcodegen_encodeText(text, tempBuffer, qrcode, errCorLvl,qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX, qrcodegen_Mask_AUTO, true);
    if (ok)
        printQr(qrcode);
}

// Prints the given QR Code to the console.
static void printQr(const uint8_t qrcode[]) {

    /* Initialisation fichier */
    const char *path2 = "..\\decode_qrcode\\qr_code_1.png";

    /* Initialisation SDL */
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Surface *screen;

    //Lancement SDL
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
        SDL_ExitWithError("Initialisation SDL");

    //Création fenêtre
    window = SDL_CreateWindow("Qr Code",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,800,800,0);
    if(window == NULL){
        SDL_ExitWithError("Creation fenetre echouee");
    }

    renderer = SDL_CreateRenderer(window, -1,SDL_RENDERER_TARGETTEXTURE); // -1 driver pour affichage

    if(renderer == NULL){
        SDL_ExitWithError("Creation rendue echouee");
    }

    /* Changer la couleur de la fenêtre */
    //SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    //SDL_RenderClear(renderer);

    /* Affichage du qr code */
    int size = qrcodegen_getSize(qrcode); // size = 49
    int border = 4;
    int new_y = 0;

    for (int y = -border; y < size + border; y++) {
        new_y = y*10+20;
        for (int x = -border; x < size + border; x++) {

            if( qrcodegen_getModule(qrcode, x, y) ){
                SDL_SetRenderDrawColor(renderer,0, 0, 0, 255);
                SDL_Rect rect;
                rect.x = x*10+20;
                rect.y = new_y;
                rect.w = 10;
                rect.h = 10;
                SDL_RenderFillRect(renderer, &rect);
                SDL_RenderPresent(renderer);
            } else {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_Rect rect;
                rect.x = x*10+20;
                rect.y = new_y;
                rect.w = 10;
                rect.h = 10;
                SDL_RenderFillRect(renderer, &rect);
                SDL_RenderPresent(renderer);
            }

        }
    }

    SDL_bool program_lanched = SDL_TRUE;

    while(program_lanched)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event) ) //recup n'importe quel evenement sans bloquer le prog
        {
            switch(event.type) //changer le type de event
            {
                case SDL_QUIT:  //pour associer le type a l'action

                    program_lanched = SDL_FALSE;

                    /* Exportation PNG */
                    screen = SDL_CreateRGBSurface(0, 750, 750, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
                    SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA32, screen->pixels, screen->pitch);
                    IMG_SavePNG(screen, path2);
                    if (IMG_SavePNG(screen,path2) != 0)
                        SDL_ExitWithError("Failed screenshot");
                    SDL_FreeSurface(screen);
                    break;

                default:
                    break;

            }
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

}

void SDL_ExitWithError(const char *message) {
    SDL_Log("ERREUR : %s > %s\n", message, SDL_GetError()); // pour eviter d'ecrire a chaque fois les log Erreur : ecran , init etc
    SDL_Quit();
    exit(EXIT_FAILURE);
}

