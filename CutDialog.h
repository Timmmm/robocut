#ifndef CUTDIALOG_H
#define CUTDIALOG_H

#include <QDialog>

namespace Ui {
    class CutDialog;
}
/*
("100", 10, 27, "yellow", "Card without Craft Paper Backing"),
("101", 10, 27, "yellow", "Card with Craft Paper Backing"),
("102", 10, 10, "blue", "Vinyl Sticker"),
("106", 10, 14, "blue", "Film Labels"),
("111", 10, 27, "yellow", "Thick Media"),
("112", 10,  2, "blue", "Thin Media"),
("113", 10, 10, "pen", "Pen"),
("120", 10, 30, "blue", "Bond Paper 13-28 lbs"),
("121", 10, 30, "yellow", "Bristol Paper 57-67 lbs"),
("122", 10, 30, "yellow", "Cardstock 40-60 lbs"),
("123", 10, 30, "yellow", "Cover 40-60 lbs"),
("124", 10,  1, "blue", "Film, Double Matte Translucent"),
("125", 10,  1, "blue", "Film, Vinyl With Adhesive Back"),
("126", 10,  1, "blue", "Film, Window With Kling Adhesive"),
("127", 10, 30, "red", "Index 90 lbs"),
("128", 10, 20, "yellow", "Inkjet Photo Paper 28-44 lbs"),
("129", 10, 27, "red", "Inkjet Photo Paper 45-75 lbs"),
("130",  3, 30, "red", "Magnetic Sheet"),
("131", 10, 30, "blue", "Offset 24-60 lbs"),
("132", 10,  5, "blue", "Print Paper Light Weight"),
("133", 10, 25, "yellow", "Print Paper Medium Weight"),
("134", 10, 20, "blue", "Sticker Sheet"),
("135", 10, 20, "red", "Tag 100 lbs"),
("136", 10, 30, "blue", "Text Paper 24-70 lbs"),
("137", 10, 30, "yellow", "Vellum Bristol 57-67 lbs"),
("138", 10, 30, "blue", "Writing Paper 24-70 lbs")
300 = custom.*/

class CutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CutDialog(QWidget *parent = 0);
    ~CutDialog();

	int media() const;
	int speed() const;
	int pressure() const;
	bool trackEnhancing() const;

protected:
    void changeEvent(QEvent *e);

private slots:
	void onMediaChanged(int idx);
private:
    Ui::CutDialog *ui;
};

#endif // CUTDIALOG_H
