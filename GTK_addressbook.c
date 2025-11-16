#include <gtk/gtk.h>
#include <string.h>

// Lisää yksi henkilö taulukkoon
static void add_person(GtkListStore *store,
                       const gchar *firstname,
                       const gchar *lastname,
                       const gchar *birthyear) {
    GtkTreeIter iter;
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter,
                       0, firstname,
                       1, lastname,
                       2, birthyear,
                       -1);
}

// Parsii yhden rivin muodossa "Etunimi Etunimi2 Sukunimi (Born: YYYY)"
static void parse_line(GtkListStore *store, const char *line) {
    char firstname[128] = {0};
    char lastname[128] = {0};
    char birthyear[16] = {0};

    // Etsitään syntymävuosi
    const char *born_ptr = strstr(line, "(Born:");
    if (born_ptr) {
        sscanf(born_ptr, "(Born: %15s", birthyear);
        // Poistetaan sulku lopusta
        char *paren = strchr(birthyear, ')');
        if (paren) *paren = '\0';
    }

    // Kopioidaan nimi ilman syntymävuotta
    char namepart[256];
    strncpy(namepart, line, born_ptr - line);
    namepart[born_ptr - line] = '\0';

    // Erotellaan sanat: viimeinen sana = sukunimi, loput = etunimi(t)
    char *last_space = strrchr(namepart, ' ');
    if (last_space) {
        strcpy(lastname, last_space + 1);
        strncpy(firstname, namepart, last_space - namepart);
        firstname[last_space - namepart] = '\0';
    } else {
        strcpy(lastname, namepart);
    }

    add_person(store, firstname, lastname, birthyear);
}

int main(int argc, char *argv[]) {
    GtkWidget *window, *view, *scrolled;
    GtkListStore *store;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    gtk_init(&argc, &argv);

    // Luodaan data store
    store = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    // TreeView
    view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));

    // Sarakkeet
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Etunimi", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(view), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Sukunimi", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(view), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Syntymävuosi", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(view), column);

    // Esimerkkituloste parsittavaksi
    parse_line(store, "Helmi Lyydia Mäkinen (Born: 1902)");
    parse_line(store, "Sofia Kerttu Laine (Born: 1908)");
    parse_line(store, "Elsa Lehtonen (Born: 1905)");

    // Vierityspalkki
    scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled), view);

    // Ikkuna
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Osoitekirja");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200);
    gtk_container_add(GTK_CONTAINER(window), scrolled);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
