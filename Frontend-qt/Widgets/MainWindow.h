#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ReverbSetting.hpp"
#include "DragDropWidget.h"

#include <QMainWindow>
#include <QStringList>
#include <QTimer>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QTemporaryFile>
#include <QNetworkAccessManager>
#include <QBuffer>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }  
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT
	
public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();

	auto fetch_and_handle_result() -> void;
	auto play_wav(const QByteArray& response_data, const QString& file_name) -> void;
	auto request_result(const std::function<void(const QByteArray&, const QString&, const QString&)> &on_ok) -> void;
	auto save_to_disk(const QByteArray& response_data, const QString& file_name) -> void;

	auto send_files_to_backend(const QStringList& paths, const ReverbSetting& settings) -> void;
	auto poll_progress() -> void;

private:
	Ui::MainWindow *ui;

	QMediaPlayer* media_player_;
	QAudioOutput* audio_output_;
	QTemporaryFile* temp_wav_file_;

	ReverbSetting reverb_settings_;
	QStringList upload_paths_;

	QTimer progress_timer_;
	QString job_id_;

	QString base_api_url_;
	QString temp_wav_file_path_;
	QNetworkAccessManager network_manager_;
	QScopedPointer<QBuffer> wav_buffer_;
};
#endif