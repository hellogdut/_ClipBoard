#include <ScreenShoot/ScreenShoot.h>
ScreenShoot* ScreenShoot::instance = 0;
ScreenShoot::ScreenShoot()
{
    // 获取屏幕尺寸
    QDesktopWidget* desktopWidget = QApplication::desktop();
    QRect deskRect = desktopWidget->screenGeometry();
    // 将窗体设置为屏幕尺寸，去掉标题栏
    g_width = deskRect.width();
    g_height = deskRect.height();
    this ->resize(g_width,g_height);

    // 调用setbackground() 设置背景
    setbackground(deskRect.width(),deskRect.height());

    //初始化变量
    g_rubber = NULL;

    origin = end = QPoint(0,0);
    label  = new QLabel();

    this ->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    label->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);


}
ScreenShoot::~ScreenShoot()
{

}
void ScreenShoot::mousePressEvent(QMouseEvent *e)
{
    if(!g_rubber)
    {
        g_rubber = new QRubberBand(QRubberBand::Rectangle,this);
    }
    // 将橡皮框设置课件
    g_rubber->show();
    // 记录点击位置
    origin = e->pos();
    // 设置橡皮框选区
    g_rubber->setGeometry(origin.x(),origin.y(),0,0);

}
void ScreenShoot::mouseMoveEvent(QMouseEvent *e)
{
    if(g_rubber)
    {
        // 记录终点
        end = e->pos();

        //调整要显示的区域
        int w = abs(end.x() - origin.x());
        int h = abs(end.y() - origin.y());
        int x = origin.x() < end.x() ? origin.x() : end.x();
        int y = origin.y() < end.y() ? origin.y() : end.y();

        g_rubber->setGeometry(x,y,w,h);
        // 动态显示选区大小
        setLabel();
    }
}
void ScreenShoot::mouseReleaseEvent(QMouseEvent *e)
{
    if(g_rubber)
    {
        // 记录终点
        end = e ->pos();
        g_rubber->hide();
        // 截屏
        grabScreen();
        // 截屏后关闭程序和标签栏
        label->close();
        // 如果使用close()，当主窗口隐藏时，主窗口也会close()
        this ->hide();
    }
}
void ScreenShoot::grabScreen()
{
    // 调整要截取的区域
    int wid = abs(origin.x() - end.x());
    int height = abs(origin.y() - end.y());
    int x = origin.x() < end.x() ? origin.x() : end.x();
    int y = origin.y() < end.y() ? origin.y() : end.y();

    //构造文件名
    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("yyyyMMddhhmmss");
    QString picName = str + ".bmp";

    // 对全屏背景进行截图
    QImage pic = bg->copy(x,y,wid,height);
    pic.save(picName);

    // 获取程序当前路径，打开截图
    QString path;
    QDir dir;
    path=dir.currentPath();
    QString picPath = "file:///" + path + "/" + picName;
    QDesktopServices::openUrl(QUrl(picPath));

    //删除背景图


}
void ScreenShoot::setLabel()
{
    // 调整标签显示位置
    int wid = abs(origin.x() - end.x());
    int height = abs(origin.y() - end.y());
    int x = origin.x() < end.x() ? origin.x() : end.x();
    int y = origin.y() < end.y() ? origin.y() : end.y();

    // 设置标签显示内容
    QString str = QString(" %1 x %2      ").arg(wid).arg(height);
    label->setText(str);
    // 设置标签位置
    QRect rect(label->contentsRect());
    label->move(QPoint(x,y - rect.height()));
    label->show();

}
void ScreenShoot::keyPressEvent(QKeyEvent *e)
{
    // 如果按下ESC键，关闭程序
    if(e->key() == Qt::Key_Escape)
    {
        label->close();
        // 如果使用close()，当主窗口隐藏时，主窗口也会close()
        this ->hide();
        QFile::remove("bg.bmp");
    }

}
void ScreenShoot::setbackground(int width,int height)
{
    //截取当前屏幕
    QScreen *screen = QGuiApplication::primaryScreen();
    QImage img = screen->grabWindow(0).toImage();

    //读取背景图，等比例缩放RGB值，使背景变暗，进入截图模式
    int red,green,blue;

    //bg = new QImage("bg.bmp");
    bg = new QImage(img);
    QImage bg_blend(width,height,QImage::Format_RGB32);
    for(int x = 0; x < width; x++)
    {
        for(int y = 0; y < height; y++)
        {
            red = qRed(bg->pixel(x,y)) * 0.6 ;
            green = qGreen(bg->pixel(x,y)) * 0.6;
            blue = qBlue(bg->pixel(x,y)) * 0.6;
            bg_blend.setPixel( x, y, qRgb( red, green, blue ) );
        }
    }
    // 将图片设置为背景

    QPalette   palette;
    palette.setBrush(this->backgroundRole(),QBrush(bg_blend));
    this->setPalette(palette);

}
void ScreenShoot::show()
{
    QWidget::show();
    setbackground(g_width,g_height);
}
