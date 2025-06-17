#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <QHttpMultiPart>
#include <QLabel>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVBoxLayout>
#include <QDebug>
#include <QNetworkReply>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent),
	  job_id_(""),
	  network_manager_{this},
	  media_player_(nullptr),
	  audio_output_(nullptr),
	  temp_wav_file_(nullptr),
	  base_api_url_("http://localhost:8080"),
	  ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	ui->ItemListWidget->setLayout(new QVBoxLayout());
	ui->ItemListWidget->setStyleSheet("border: 1px solid black;");

	media_player_ = new QMediaPlayer(this);
	audio_output_ = new QAudioOutput(this);

	media_player_->setAudioOutput(audio_output_);

	connect(ui->DragAndDropWidget, &DragDropWidget::filesDropped, this, [&](const QStringList &paths)
			{
		for (const QString &path : paths) 
		{
			QFile file (path);
			if (!file.exists())
			{
				continue;
			}
			upload_paths_ << path;
		 	QLabel *label = new QLabel(file.fileName());
			label->setWordWrap(true);
			label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			ui->ItemListWidget->layout()->addWidget(label);
		} });

	connect(ui->RenderStart, &QPushButton::clicked, this, [&]()
			{ send_files_to_backend(upload_paths_, reverb_settings_); });

	connect(ui->RoomSizeSlider, &QSlider::valueChanged, this, [&](int value)
			{
		reverb_settings_.room_size_ = value;
		ui->label_2->setText(QString("Room Size: %1").arg(value)); });

	connect(ui->DampingSlider, &QSlider::valueChanged, this, [&](int value)
			{
		reverb_settings_.damping_ = value;
		ui->label_3->setText(QString("Damping: %1").arg(value)); });

	connect(ui->WetLevelSlider, &QSlider::valueChanged, this, [&](int value)
			{
		reverb_settings_.wet_level_ = value;
		ui->label_4->setText(QString("WetLevel: %1").arg(value)); });

	connect(ui->DryLevelSlider, &QSlider::valueChanged, this, [&](int value)
			{
		reverb_settings_.dry_level_ = value;
		ui->label_5->setText(QString("DryLevel: %1").arg(value)); });

	connect(ui->WidthSlider, &QSlider::valueChanged, this, [&](int value)
			{
		reverb_settings_.width_ = value;
		ui->label_6->setText(QString("Width: %1").arg(value)); });

	connect(ui->ResultPlayButton, &QPushButton::clicked, this, [this](){
		request_result([this](const QByteArray &data, const QString &ctype, const QString &name){
			if (ctype.startsWith("audio") || (ctype.contains("octet") && name.endsWith(".wav", Qt::CaseInsensitive)))
			{
				play_wav(data, name);
			}
			else
			{
				QMessageBox::warning(this, "오류", "다중 파일(zip)은 미리듣기를 지원하지 않습니다.");
			}
		});
	});

	connect(ui->ResultSaveButton, &QPushButton::clicked, this, [this]() {
		request_result([this](const QByteArray &data, const QString &, const QString &name)
		{
			save_to_disk(data, name);
		});
	});

	connect(&progress_timer_, &QTimer::timeout, this, &MainWindow::poll_progress);
	progress_timer_.setInterval(1500);
	progress_timer_.setSingleShot(false);

	ui->RoomSizeSlider->setValue(50);
	ui->DampingSlider->setValue(50);
	ui->WetLevelSlider->setValue(50);
	ui->DryLevelSlider->setValue(50);
	ui->WidthSlider->setValue(50);
}

MainWindow::~MainWindow()
{
	delete ui;

	if (QFile::exists(temp_wav_file_path_))
	{
		QFile::remove(temp_wav_file_path_);
	}
}

auto MainWindow::fetch_and_handle_result() -> void
{
	if (job_id_.isEmpty())
	{
		qDebug() << "No job id";
		return;
	}

	QNetworkRequest request(QUrl(QString(base_api_url_ + "/api/render/%1/result").arg(job_id_)));
	auto *reply = network_manager_.get(request);

	connect(reply, &QNetworkReply::finished, this, [this, reply]()
			{
		if (reply->error() != QNetworkReply::NoError)
		{
			qDebug() << "Error fetching result:" << reply->errorString();
			reply->deleteLater();
			return;
		}

		const QByteArray response_data = reply->readAll();
		const QString content_type = reply->header(QNetworkRequest::ContentTypeHeader).toString().toLower();
		const QString cd = reply->rawHeader("Content-Disposition");
		const QString file_name = QRegularExpression("filename=\"?([^\";]+)\"?").match(cd).captured(1);

		if (content_type.startsWith("audio") || content_type.contains("octet-stream") && file_name.endsWith(".wav", Qt::CaseInsensitive))
		{
			if (temp_wav_file_)
			{
				temp_wav_file_->deleteLater();
				temp_wav_file_ = nullptr;
			}

			temp_wav_file_ = new QTemporaryFile(QDir::tempPath() + "/reverb_result_XXXXXX.wav", this);
			if (temp_wav_file_->open())
			{
				temp_wav_file_->write(response_data);
				temp_wav_file_->flush();
				temp_wav_file_->close();
			}

			media_player_->setSource(QUrl::fromLocalFile(temp_wav_file_->fileName()));
			qDebug() << "Playing audio from:" << temp_wav_file_->fileName();
			audio_output_->setVolume(1.0);
			media_player_->play();

			const QString save_path = QFileDialog::getSaveFileName(this, tr("Save Rendered File"),
											QDir::homePath() + "/" + file_name,
											tr("WAV Files (*.wav);;All Files (*)"));
			if (!save_path.isEmpty())
			{
				QFile file(save_path);
				if (file.open(QIODevice::WriteOnly))
				{
					file.write(response_data);
				}
			}
		}
		else
		{
			const QString save_path = QFileDialog::getSaveFileName(this, "ZIP save", file_name.isEmpty() ? "result.zip" : file_name, "ZIP Files (*.zip);;All Files (*)");
			if (!save_path.isEmpty())
			{
				QFile file(save_path);
				if (file.open(QIODevice::WriteOnly))
				{
					file.write(response_data);
				}
			}
		}

		reply->deleteLater(); });
}

auto MainWindow::play_wav(const QByteArray &response_data, const QString &file_name) -> void
{
	media_player_->stop();

	if (!temp_wav_file_path_.isEmpty())
	{
		QFile::remove(temp_wav_file_path_);
		temp_wav_file_path_.clear();
	}

	temp_wav_file_path_ = QDir::tempPath() + "/reverb_result_" + file_name;

	QFile tmp(temp_wav_file_path_);
	if (!tmp.open(QIODevice::WriteOnly))
	{
		QMessageBox::warning(this, "오류", "파일을 저장할 수 없습니다: {" + temp_wav_file_path_ + "}" + tmp.errorString());
		return;
	}
	tmp.write(response_data);
	tmp.flush();
	tmp.close();

	if (!QFile::exists(temp_wav_file_path_))
	{
		QMessageBox::warning(this, "오류", "파일이 존재하지 않습니다: " + temp_wav_file_path_);
		return;
	}
	media_player_->setSource(QUrl::fromLocalFile(temp_wav_file_path_));
	audio_output_->setVolume(100);
	media_player_->play();

	connect(media_player_, &QMediaPlayer::errorOccurred, this, [&](QMediaPlayer::Error e) {
		statusBar()->showMessage("오류 발생: " + media_player_->errorString(), 3000);
	});

	statusBar()->showMessage(file_name + " 재생 중…", 5000);
}

auto MainWindow::request_result(const std::function<void(const QByteArray &, const QString &, const QString &)> &on_ok) -> void
{
	if (job_id_.isEmpty())
	{
		return;
	}

	QNetworkRequest req(QUrl(base_api_url_ + "/api/render/" + job_id_ + "/result"));
	auto *reply = network_manager_.get(req);

	connect(reply, &QNetworkReply::finished, this,
			[this, reply, on_ok]()
			{
				if (reply->error() != QNetworkReply::NoError)
				{
					qWarning() << "result fetch error:" << reply->errorString();
					reply->deleteLater();
					return;
				}

				QByteArray data = reply->readAll();
				QString ctype = reply->header(QNetworkRequest::ContentTypeHeader).toString().toLower();
				QString cd = reply->rawHeader("Content-Disposition");
				QString filename = QRegularExpression("filename=\"?([^\";]+)\"?").match(cd).captured(1);

				on_ok(data, ctype, filename);
				reply->deleteLater();
			});
}

auto MainWindow::save_to_disk(const QByteArray &response_data, const QString &file_name) -> void
{
	QString dlgName = file_name.isEmpty() ? "result.wav" : file_name;
	QString filter = dlgName.endsWith(".zip", Qt::CaseInsensitive)
						 ? "ZIP Files (*.zip);;All Files (*)"
						 : "WAV Files (*.wav);;All Files (*)";

	QString path = QFileDialog::getSaveFileName(this, "파일 저장", QDir::homePath() + "/" + dlgName, filter);
	if (path.isEmpty())
	{
		return;
	}

	QFile out(path);
	if (out.open(QIODevice::WriteOnly))
	{
		out.write(response_data);
	}
}

auto MainWindow::send_files_to_backend(const QStringList &paths, const ReverbSetting &settings) -> void
{
	if (paths.isEmpty())
	{
		qDebug() << "No files to upload.";
		return;
	}

	auto *multi = new QHttpMultiPart(QHttpMultiPart::FormDataType);

	for (const auto &p : paths)
	{
		QHttpPart part;
		part.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("audio/*"));
		part.setHeader(QNetworkRequest::ContentDispositionHeader,
					   QString("form-data; name=\"files\"; filename=\"%1\"").arg(QFileInfo(p).fileName()));

		QFile *file = new QFile(p, multi);
		if (!file->open(QIODevice::ReadOnly))
		{
			delete file;
			continue;
		}
		part.setBodyDevice(file);
		multi->append(part);
	}

	QHttpPart json_part;
	json_part.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
	json_part.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"params\"");

	json_part.setBody(QJsonDocument(settings.to_json()).toJson());
	multi->append(json_part);

	QNetworkRequest request(QUrl(base_api_url_ + "/api/render"));
	auto *reply = network_manager_.post(request, multi);
	multi->setParent(reply);

	connect(reply, &QNetworkReply::finished, this, [reply, this]()
			{
        if (reply->error() != QNetworkReply::NoError)
		{
            qWarning() << "Upload failed:" << reply->errorString();
		}
        else
        {
            const QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            job_id_ = doc["job_id"].toString();
			ui->RenderProgressBar->setValue(0);
			progress_timer_.start();
        }

        reply->deleteLater(); });
}

auto MainWindow::poll_progress() -> void
{
	if (job_id_.isEmpty())
	{
		return;
	}

	QNetworkRequest request(QUrl(QString(base_api_url_ + "/api/render/%1").arg(job_id_)));
	auto *reply = network_manager_.get(request);

	connect(reply, &QNetworkReply::finished, this, [this, reply]()
			{
		if (reply->error() != QNetworkReply::NoError)
		{
			qDebug() << "progress poll error:" << reply->errorString();
			progress_timer_.stop();
			qDebug() << "Stopping progress timer.";
			reply->deleteLater();
			return;
		}

		const QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
		const QJsonObject obj = doc.object();

		int pct = 0;
		if (obj.contains("progress"))
		{
			pct = obj["progress"].toInt();
		}
		else if (obj.contains("percent"))
		{
			pct = obj["percent"].toInt();
		}
		else if (obj.isEmpty() && doc.isObject())
		{
			pct = doc.toVariant().toInt();
		}

		if (pct < 0 || obj.contains("error"))
		{
			progress_timer_.stop();
			ui->RenderProgressBar->setValue(0);
			reply->deleteLater();
			return;
		}

		ui->RenderProgressBar->setValue(pct);

		if (pct >= 100)
		{
			progress_timer_.stop();
			qDebug() << "Render complete.";
		}

		reply->deleteLater(); });
}
