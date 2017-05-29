# QVFB2

This is a virtual frame buffer program running on Qt.

This program (qvfb2) is compliant to MiniGUI V3.0's XVFB specification.

Run the following commands to install qvfb2.

	$ ./configure --with-qt-includes=<path_to_qt3_include_dir> --with-qt-libs=<path_to_qt3_lib_dir>
	$ make
	$ sudo make install

And then you should change something in MiniGUI.cfg:

	[system]
	gal_engine=pc_xvfb

	[pc_xvfb]
	exec_file=/usr/local/qvfb2

To enable system input method support:

	$ export QT_IM_MODULE=scim-bridge

and you need to install scim-bridge-qt package
