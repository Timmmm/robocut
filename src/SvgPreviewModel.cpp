#include "SvgPreviewModel.h"

#include <QFileInfo>

#include "SvgRenderer.h"

SvgPreviewModel::SvgPreviewModel(QObject* parent) : QAbstractListModel(parent)
{
}

QVariant SvgPreviewModel::data(const QModelIndex& index, int role) const
{
	if (index.row() < 0 || index.row() >= entries.size())
		return {};

	switch (role)
	{
	case Qt::DisplayRole:
		return QFileInfo(entries[index.row()].filename).baseName();

	case Qt::DecorationRole:
		return entries[index.row()].preview;

	case Qt::ToolTipRole:
		return entries[index.row()].filename;

	case FilenameRole:
		return entries[index.row()].filename;

	case Qt::SizeHintRole:
		return entries[index.row()].preview.size();
	}

	return {};
}

Qt::ItemFlags SvgPreviewModel::flags(const QModelIndex& index) const
{
	(void)index;

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
}

int SvgPreviewModel::rowCount(const QModelIndex& parent) const
{
	(void)parent;

	return static_cast<int>(entries.size());
}

void SvgPreviewModel::setFiles(const QStringList& filenames)
{
	beginResetModel();

	entries.clear();

	for (const auto& fn : filenames)
	{

		// TODO: Render in another thread.
		QPixmap preview = svgToPreviewImage(fn, {previewSize, previewSize});

		entries.append({fn, preview});
	}

	endResetModel();
}

void SvgPreviewModel::setPreviewSize(int px)
{
	previewSize = px;
}
