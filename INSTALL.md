## ビルド手順

まず必要なライブラリが入っているかどうかを確認して下さい。

  - Bullet Physics
  - OpenGL Easy Extension
  - Qt 4.6 以降

プラグインをビルドする際以下が必要です。Phonon は Qt に含まれれます。

  - Phonon (QMAAudioPlugin)
  - AquesTalk2 (QMAAquesTalk2Plugin)
  - Julius (QMAJuliusPlugin)
  - OpenJTalk (QMAOpenJTalkPlugin)
  - PortAudio (QMAAquesTalk2Plugin / QMAJuliusPlugin / QMAOpenJTalkPlugin)
  - hts_engine (QMAOpenJTalkPlugin)
  - Lua (QMAVILuaPlugin)

MMDAI 及び MMDME のビルドに cmake が必要です。MacOSX の場合はさらに Xcode が必要です。
Xcode をインストールしないと gcc 等が入らないので必ずインストールしてください。

MacOSX の場合 OpenGL Easy Extension が正しくビルド出来無い問題があるため、
libGLEE に入っている CMakeList.txt を入れて cmake を使ってビルドする必要があります。
インストール先は /usr または /usr/local にインストールする必要があります。
Ubuntu Linux の場合、apt-get でインストールした方が楽です。

## 環境変数の設定

以下の環境変数を指定するとその値からライブラリ及びヘッダーの検出を行います。MSVC でビルドする場合は必須です。
Windows で環境設定する際はユーザの環境変数を用いるとよいでしょう(注意点として更新の反映は一度ログオフして再度ログインする必要がある)。

  - GLEE_DIR: GLee.lib のソースがあるパス指定
  - MMDAI_LIBRARY_DIR: libMMDAI.(dylib|so) または MMDAI.lib がインストールされているパスを指定
  - MMDME_LIBRARY_DIR: libMMDME.(dylib|so) または MMDME.lib がインストールされているパスを指定
  - BULLET_LIBRARY_DIR: libBullet*.(dylib|so) または Bullet*.lib がインストールされているパスを指定
  - MMDAI_INCLUDE_DIR: MMDAI/PMDObject.h があるパスを指定 (この場合 MMDAI のディレクトリがあるパスを指定)
  - MMDME_INCLUDE_DIR: MMDME/MMDME.h があるパスを指定 (この場合 MMDME のディレクトリがあるパスを指定)
  - BULLET_INCLUDE_DIR: btBulletDynamicsCommon.h があるパスを指定

MSVC でビルドする場合 BULLET_INCLUDE_DIR は Bullet のトップディレクトリ以下の src を指定します。
MMDAI_INCLUDE_DIR 及び MMDME_INCLUDE_DIR ディレクトリはトップディレクトリ以下の MMDAI/include と
MMDME/include を指定します。

  OpenGL Easy Extension を CMake でビルドする場合

    $ cp $MMDAI/libGLEE/CMakeFiles.txt $GLEE_SRC_DIR
    $ mv GLee.c glee.c
    $ cmake .
    $ make
    $ sudo make install

## Linux か MacOSX でビルドする場合

    $ cd libMMDME
    # 共有ライブラリを作成する場合。共有ライブラリではなく、静的ライブラリを作成する場合は
    # -DBUILD_SHARED_LIBS=OFF にするか、または単純に "cmake ." とするだけでよい。
    $ cmake -DBUILD_SHARED_LIBS=ON .
    $ make 
    $ sudo make install
    $ cd ../libMMDAI
    $ cmake .
    $ make
    $ sudo make install
    $ cd ..
    $ qmake
    $ make # MacOSX の場合は xcodebuild

## MSVC でビルドする場合 (暫定。記述が間違っている箇所があると思われる)

まずは Visual Studio 2008 をインストールする必要があります。Express C++ でも可能です。
(Visual Studio 2010 は Qt ライブラリの関係でビルドが正しく行えない)

次に Qt の Visual Studio ビルド版と CMake をインストールし、Bullet のソースコードを入手します。
両者をインストールしたら CMake を立ち上げ、先程の Bullet のトップディレクトリ上を指定して
"Visual Studio 9 2008" 形式でプロジェクトファイルを生成します。
デモアプリの生成を行わないように設定しておくとビルド時間を短縮することが可能です。

ビルドした Bullet のパスを上記の環境変数を設定し、同じように MMDME を CMake で
プロジェクトファイルを生成します。生成したら MMDME のパスを環境変数に設定し、
MMDAI のプロジェクトファイルを生成、ビルドを行います。

QtMMDAI のファイル生成は qmake で行います。コマンドラインから "qmake -tp vc" を実行し、プロジェクトファイルを生成します。
ソリューションファイル (sln) は作成されないので、ダイアログが出てきたら保存してください。ビルドする前に QtMMDAI のプロジェクトの
プロパティを開き、「構成プロパティ」->「リンカー」->「入力」にある「特定の既定のライブラリの無視」の項目に"libc.lib" を追加してください。

上記過程を終わったら QtMMDAI をビルドしてください。ビルドが正常に完了すると debug または release ディレクトリの下に
QtMMDAI.exe が出来上がります。

## iPhone 向けのビルド (まだシミュレータ上のみのため、途中の段階)

前提条件として Xcode4 を用いるため、それ向けの説明を行います。
まず Bullet を cmake で Xcode プロジェクトファイルを生成するようにします。

    $ cmake -G Xcode

複数のターゲットが表示されますが、使用するターゲットは以下の4つのみです。

  - BulletSoftBody
  - BulletCollision
  - BulletDynamics
  - LinearMath

これらを以下の手順でビルドします。

  - "Base SDK" を "Latest iOS" に変更する
  - "Architectures" を "Optimized (arm7)" にする
  - CTRL + B でビルドを実行

MMDAI 及び MMDME も同じように cmake で Xcode を生成し、SDK の選択を変更する流れで同じです。

## MinGW (現在この方法でビルドは行っていないため、記述が古い)

Linux 上でクロスコンパイルを行います。yum で MinGW の開発環境が揃えられる
Fedora Linux でビルドしたほうがよいかもしれません。
以下の解説は Fedora Linux 14 を使った場合でのビルド方法です。

### OpenGL Easy Extension

cmake を使います。mingw32-cmake を使うこと以外方法は同じです。

### Bullet Physics

一旦トップディレクトリで mingw32-cmake を実行します。その後 src ディレクトリに移動し、
make を実行します。ビルド出来たら make install を実行します。

### MMDME / MMDAI

mingw32-cmake を使うこと以外方法は同じです。OpenGL Easy Extension と
Bullet Physics の二つがインストールされていることが前提条件です。

### QtMMDAI

qmake-qt4 の代わりに mingw32-qmake-qt4 を使う必要があります。
mingw32-qmake-qt4 を実行し、make すればバイナリが作成されます。
生成されたバイナリは debug または release の QtMMDAI.exe になります。

実行する際 dll が必要なため、配置スクリプトを実行します。
配置スクリプトの第一引数に "-release" つけることで Qt のリリースビルドをリンクします。
さらに第二引数に "-deploy" つけることで dll をコピーしてパッケージを作成します。

    $ cd debug # or release
    $ perl ../scripts/mingw_deloy.pl
    $ wine QtMMDAI.exe

## ビルドの注意点

OpenJTalk について取り扱いに注意が必要です。まず、OpenJTalk のビルドが終わっても
そのままビルドすることは出来ません。QMAOpenJTalkPlugin.pro にある手順を行う必要があります。
また、以下を行わないと実行時に SEGFAULT を起こします。

  - CXXFLAGS に -DMECAB_WITHOUT_SHARE_DIC をつける
  - "./configure --with-charset=SHIFT_JIS" で実行する

    $ export CXXFLAGS="-DMECAB_WITHOUT_SHARE_DIC"; ./configure --with-charset=SHIFT_JIS
