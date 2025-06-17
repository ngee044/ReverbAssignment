#pragma once

#include <QWidget>
#include <QStringList>
#include <QMimeData>
#include <QStringList>

class DragDropWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DragDropWidget(QWidget *parent = nullptr);

	auto hasAudio(const QMimeData *mime) -> bool const;
signals:
    auto filesDropped(const QStringList &paths) -> void;

protected:
    auto dragEnterEvent(QDragEnterEvent *e)  -> void override;
    auto dropEvent(QDropEvent *e)  -> void override;

private:
};
