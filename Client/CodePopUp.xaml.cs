////////////////////////////////////////////////////////////////////////////////
//  CodePopUp.xaml.cs - Functionality to pop up a window                      //
//  ver 1.0																	  //
//  Language:      .NET, C#, Visual Studio 2017							      //
//  Platform:      Dell XPS, Windows 10     								  //
//  Application:   Helper for WPF application for CSE687 - OOD Project 3	  //
//  Author:		   Vinayak Kukreja, Syracuse University						  //
////////////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
* =======================
*  This package defines a pop up window that is a helper for our WPF application
*
*
* Public Interface:
* -----------------
*  public partial class CodePopUp : Window:

	public CodePopUp()
		--> Constructor- Initializing components here
*
*  Required Files:
* =======================
*  CodePopUp.xaml CodePopUp.xaml.cs
*
*  Maintainence History:
* =======================
*  ver 1.0 - 10 April 2019
*  - Author
*/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace CodePublisherClient
{
    // -----< Partial class MainWindow which hosts our pop up window >------------------------------------
    public partial class CodePopUp : Window
    {
        // -----< Constructor- Initializing components here >------------------------------------
        public CodePopUp()
        {
            InitializeComponent();
        }
    }
}
