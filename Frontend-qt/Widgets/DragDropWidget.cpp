#include "DragDropWidget.h"

#include <QDragEnterEvent>
#include <QPainter>

DragDropWidget::DragDropWidget(QWidget *parent)
	: QWidget(parent)
{
	setAcceptDrops(true);
	setStyleSheet("background:#fafafa; border:2px dashed #8aa;");
}

auto DragDropWidget::hasAudio(const QMimeData *mime) -> bool const
{
	static const QStringList exts = {".wav", ".mp3"};
	if (!mime->hasUrls())
	{
		return false;
	}

	for (const QUrl &url : mime->urls())
	{
		const QString path = url.toLocalFile().toLower();
		for (const auto &ext : exts)
		{
			if (path.endsWith(ext))
			{
				return true;
			}
		}
	}
	return false;
}

auto DragDropWidget::dragEnterEvent(QDragEnterEvent *e) -> void
{
	if (hasAudio(e->mimeData()))
	{
		e->acceptProposedAction();
	}
}

auto DragDropWidget::dropEvent(QDropEvent *e) -> void
{
	QStringList paths;
	for (const QUrl &url : e->mimeData()->urls())
	{
		if (url.isLocalFile())
		{
			paths << url.toLocalFile();
		}
	}

	if (!paths.isEmpty())
	{
		emit filesDropped(paths);
	}

	e->acceptProposedAction();

}
