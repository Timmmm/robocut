#include "SvgPreviewModel.h"

#include <QFileInfo>

#include "SvgRenderer.h"

SvgPreviewModel::SvgPreviewModel(int previewSize_, QObject* parent)
	: QAbstractListModel(parent), previewSize(previewSize_)
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
	
	return entries.size();
}

void SvgPreviewModel::setFiles(const QStringList& filenames)
{
	beginResetModel();
	
	entries.clear();
	
	for (const auto& fn : filenames) {
		
		QPixmap preview;
		
		QSvgRenderer renderer;
		if (renderer.load(fn))
			preview = svgToPreviewImage(renderer, {previewSize, previewSize});

		// TODO: Render in another thread.
		entries.append({fn, preview});
	}
	
	endResetModel();
}
