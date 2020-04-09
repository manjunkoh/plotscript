#include <QApplication>
#include "input_widget.hpp"
#include "output_widget.hpp"
#include "notebook_app.hpp"


int main(int argc, char *argv[])
{
  QApplication app(argc, argv);


  NotebookApp plotscript;
  plotscript.show();
  
  //myOutput.show();
  
  return app.exec();
}

