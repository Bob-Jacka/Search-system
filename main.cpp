#include "core/entities/UI/Interface.hpp"

import Libio;
#include <QMessageBox>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    auto main_window = std::make_unique<Ui::MainWindow>();
    auto main_container_win = std::make_unique<QMainWindow>();

    main_window->setupUi(main_container_win.get());

    //Window entities:
    auto search_btn = main_container_win->findChild<QPushButton *>("search_btn");
    auto search_txt_field = main_container_win->findChild<QTextEdit *>("to_search");
    auto txt_view = main_container_win->findChild<QListView *>("results");

    QPushButton::connect(search_btn, &QPushButton::clicked, [&search_txt_field, txt_view] {
        QMessageBox(QMessageBox::Icon::Warning, "Warning", "Video is not stopped").exec();
    });

    if (main_container_win) {
        main_container_win->show();
    }

    return QApplication::exec();
}
