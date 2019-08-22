////////////////////////////////////////////////////////////////////////////////
//  MainWindow.xaml.cs - GUI Functionality to convert and publish files       //
//                       matching specific patterns and regex present         //
//                       at a remote server.                                  //
//  ver 1.1																	  //
//  Language:      .NET, C#, Visual Studio 2017							      //
//  Platform:      Dell XPS, Windows 10     								  //
//  Application:   WPF application for CSE687 - OOD Project 4				  //
//  Author:		   Vinayak Kukreja, Syracuse University						  //
////////////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
* =======================
*  This package defines a WPF application that provides the functionality to convert
*  and publish files matching specific patterns and regex present at a remote server.
*
*
* Public Interface:
* -----------------
*  public partial class MainWindow : Window:

	public MainWindow()
		--> Constructor- Initializing components here
*
*  Required Files:
* =======================
*  MainWindow.xaml MainWindow.xaml.cs
*  CodePopUp.xaml CodePopUp.xaml.cs
*  Translator.h Translator.cpp
 * CsMessage.h
*
*  Maintainence History:
* =======================
*  ver 1.0 - 10 April 2019
*  - Author
*  ver 1.1 - 01 May 2019
*  - Author
*/

using MsgPassingCommunication;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Threading;
using System.Windows;
using System.Windows.Input;
using KeyEventArgs = System.Windows.Input.KeyEventArgs;
using MouseEventArgs = System.Windows.Input.MouseEventArgs;
using Path = System.IO.Path;
using SelectionMode = System.Windows.Controls.SelectionMode;

namespace CodePublisherClient
{
    // -----< Partial class MainWindow which hosts our design >------------------------------------
    public partial class MainWindow : Window
    {
        private string WorkingDirectory { get; set; }
        private string Patterns { get; set; }
        private string Regex { get; set; }
        private string ConvertedWebPagesDirectory { get; set; }
        private int flag = 1;
        private Stack<string> pathStack_ = new Stack<string>();
        private Translater translater;
        private CsEndPoint endPoint_;
        private Thread rcvThrd = null;
        private Dictionary<string, Action<CsMessage>> dispatcher_ = new Dictionary<string, Action<CsMessage>>();
        private String[] arguments;
        private string selectedFile = string.Empty;
        private string[] connectionURLClient;
        private string[] connectionURLServer;
        private string ClientUrl = string.Empty;
        private string ServerUrl = string.Empty;
        private int ClientPort = 0;
        private int ServerPort = 0;

        // -----< Constructor- Initializing components here >------------------------------------
        public MainWindow()
        {
            InitializeComponent();
            base.Closing += this.MainWindow_Closing;
        }

        // -----< Functionality to take place when window is loaded: Defining and initializing path, patterns, regex and calling the automatic test suite >------------------------------------
        private void MainWindow_OnLoaded(object sender, RoutedEventArgs e)
        {
            arguments = Environment.GetCommandLineArgs();
            connectionURLClient = arguments[1].Split(':');
            connectionURLServer = arguments[2].Split(':');
            ClientUrl = connectionURLClient[0];
            ClientPort = Int32.Parse(connectionURLClient[1]);
            ServerUrl = connectionURLServer[0];
            ServerPort = Int32.Parse(connectionURLServer[1]);
            Console.Title = "Client " + ClientUrl + ":" + ClientPort;
            Console.WriteLine("\n--------------------------------------------------------------------------------------");
            Console.WriteLine("-----------------------< Demonstrating Project-4 Requirements >-----------------------");
            Console.WriteLine("--------------------------------------------------------------------------------------");
            Console.WriteLine("\n\t\t--> Loading the GUI right now");
            ConvertedWebPagesDirectory = "../../../../ReceivedFilesFromServer";
            WorkingDirectory = "../../../../";
            WorkingDirectory = Path.GetFullPath(WorkingDirectory);
            ConvertedWebPagesDirectory = Path.GetFullPath(ConvertedWebPagesDirectory);
            Patterns = "*.h *.cpp";
            Regex = "[L](.*)";
            txtPatterns.Text = Patterns;
            Console.WriteLine("\t\t--> The current patterns are: " + Patterns);
            txtRegex.Text = Regex;
            Console.WriteLine("\t\t--> The current regex are: " + Regex);
            Console.WriteLine("\t\t--> Calling the Automatic Test Suite Function now!!");
            ConnectingToServer();
            loadDispatcher();
            loadConvertedFilesList();
            recieveFilesFromServer();
            endPoint_ = new CsEndPoint();
            endPoint_.machineAddress = ClientUrl;
            endPoint_.port = ClientPort;
            translater = new Translater();
            translater.listen(endPoint_);
            processMessages();
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = ServerUrl;
            serverEndPoint.port = ServerPort;
            pathStack_.Push("../");
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getDirs");
            msg.add("path", pathStack_.Peek());
            translater.postMessage(msg);
            msg.remove("command");
            msg.add("command", "getFiles");
            msg.add("patterns", txtPatterns.Text);
            translater.postMessage(msg);
            AutomaticTestSuite();
        }

        // -----< Automatic Test Suite: To demonstrate all requirements on startup without any user interaction >------------------------------------
        private void AutomaticTestSuite()
        {
            DemoReqOne();
            DemoReqTwo();
            DemoReqThree();
            DemoReqFour();
            DemoReqFive();
            DemoReqSix();
            Auto_Connect();
            Auto_Publish();
        }

        // -----< Functionality to connect client and server >------------------------------------
        private void Auto_Connect()
        {
            if (flag == 1)
            {
                Console.WriteLine("\n\n\t--> Auto-Connecting Client and Server");
                Console.WriteLine("\t------------------------------------------------------------\n");
                Console.WriteLine("\t\t--> Clicked the Connect button in the GUI");
                Console.WriteLine("\t\t\t--> Client URL: " + arguments[1] + " and ServerURL: " + arguments[2]);
                txtProgress.Text = "Client URL: " + arguments[1] + " and ServerURL: " + arguments[2];
            }
            else
            {
                Console.WriteLine("\t\t--> Clicked the Connect button in the GUI");
                Console.WriteLine("\t\t\t--> Client URL: " + arguments[1] + " and ServerURL: " + arguments[2]);
                txtProgress.Text = "Client URL: " + arguments[1] + " and ServerURL: " + arguments[2];
            }
            btnPublish.IsEnabled = true;
            menuPublish.IsEnabled = true;
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = ServerUrl;
            serverEndPoint.port = ServerPort;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "clientConnectionRequest");
            translater.postMessage(msg);
            btnConnect.IsEnabled = false;
        }

        // -----< Functionality to take place when Connect button is clicked >------------------------------------
        private void BtnConnect_OnClick(object sender, RoutedEventArgs e)
        {
            Auto_Connect();
        }

        //----< Functionality to process incoming messages on child thread >----------------
        private void processMessages()
        {
            ThreadStart thrdProc = () =>
            {
                while (true)
                {
                    CsMessage msg = translater.getMessage();
                    string msgId = msg.value("command");
                    if (dispatcher_.ContainsKey(msgId))
                    {
                        dispatcher_[msgId].Invoke(msg);
                    }
                }
            };
            rcvThrd = new Thread(thrdProc);
            rcvThrd.IsBackground = true;
            rcvThrd.Start();
        }

        //----< To add client processing for receiving message with key >---------------
        private void addClientProc(string key, Action<CsMessage> clientProc)
        {
            dispatcher_[key] = clientProc;
        }

        // -----< To set client port value in GUI >------------------------------------
        private void setClientPort(string clientURL)
        {
            txtClientPort.Text = "";
            txtClientPort.Text = clientURL;
        }

        // -----< To set server port value in GUI >------------------------------------
        private void setServerPort(string serverURL)
        {
            txtServerPort.Text = "";
            txtServerPort.Text = serverURL;
        }

        // -----< Functionality to take place while connecting to server >------------------------------------
        private void ConnectingToServer()
        {
            Action<CsMessage> serverConnectionRequest = (CsMessage rcvMsg) =>
            {
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("to"))
                    {
                        Action setClientPort = () =>
                        {
                            this.setClientPort(enumer.Current.Value);
                        };
                        Dispatcher.Invoke(setClientPort, new Object[] { });

                    }
                    if (key.Contains("from"))
                    {
                        Action setServerPort = () =>
                        {
                            this.setServerPort(enumer.Current.Value);
                        };
                        Dispatcher.Invoke(setServerPort, new Object[] { });
                    }
                }
            };
            addClientProc("serverConnectionRequest", serverConnectionRequest);
        }

        // -----< To clear directories in Navigation List >------------------------------------
        private void clearDirs()
        {
            lstFiles.Items.Clear();
        }

        //----< To add directories in Navigation List >-------
        private void addDir(string dir)
        {
            lstFiles.Items.Add("[" + dir + "]");
        }

        //----< To insert parent directory in Navigation List >-------
        private void insertParent()
        {
            lstFiles.Items.Insert(0, "..");
        }

        //----< To add files in Navigation List >-------
        private void addFile(string file)
        {
            lstFiles.Items.Add(file);
        }

        // -----< Functionality to take place when receives directories >------------------------------------
        private void DispatcherLoadGetDirs()
        {
            Action<CsMessage> getDirs = (CsMessage rcvMsg) =>
            {
                Action clrDirs = () =>
                {
                    clearDirs();
                };
                Dispatcher.Invoke(clrDirs, new Object[] { });

                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("dir"))
                    {
                        Action<string> doDir = (string dir) =>
                        {
                            addDir(dir);
                        };
                        Dispatcher.Invoke(doDir, new Object[] { enumer.Current.Value });
                    }
                }
                Action insertUp = () =>
                {
                    insertParent();
                };
                Dispatcher.Invoke(insertUp, new Object[] { });
            };
            addClientProc("getDirs", getDirs);
        }

        //----< Functionality to take place when receives files list from server >------
        private void DispatcherLoadGetFiles()
        {
            Action<CsMessage> getFiles = (CsMessage rcvMsg) =>
            {
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("file"))
                    {
                        Action<string> doFile = (string file) =>
                        {
                            addFile(file);
                        };
                        Dispatcher.Invoke(doFile, new Object[] { enumer.Current.Value });
                    }
                }
            };
            addClientProc("getFiles", getFiles);
        }

        //----< To set Directories and Files recieved from Server in Navigation List >---------------------------
        private void loadDispatcher()
        {
            DispatcherLoadGetDirs();
            DispatcherLoadGetFiles();
        }

        //----< Strip off name of first part of path >---------------------
        private string removeFirstDir(string path)
        {
            string modifiedPath = path;
            int pos = path.IndexOf("/");
            modifiedPath = path.Substring(pos + 1, path.Length - pos - 1);
            return modifiedPath;
        }

        // -----< Functionality to take place when an item in Navigation List is double-left-clicked: Enters the directory and does nothing when done on a file >------------------------------------
        private void LstFiles_OnMouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            Console.WriteLine("\n\t\t--> Double Clicked an Item in the Navigation List Box");
            string selectedDir = (string)lstFiles.SelectedItem;
            if (selectedDir == null)
            {
                return;
            }
            string path;
            if (selectedDir == "..")
            {
                if (pathStack_.Count > 1)
                {
                    pathStack_.Pop();
                }
                else
                {
                    txtPath.Text = "Can not go above this directory!! Server Browsing Restriction!!";
                    txtProgress.Text = "Can not go above this directory!! Server Browsing Restriction!!";
                    return;
                }
            }
            else if (selectedDir.Contains("[") && selectedDir.Contains("]"))
            {
                selectedDir = selectedDir.Substring(1, selectedDir.Length - 2);
                path = pathStack_.Peek() + "/" + selectedDir;
                pathStack_.Push(path);
            }
            txtPath.Text = removeFirstDir(pathStack_.Peek());
            txtProgress.Text = "Parent Directory is: " + Path.GetFileName(txtPath.Text);
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = ServerUrl;
            serverEndPoint.port = ServerPort;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getDirs");
            msg.add("path", pathStack_.Peek());
            translater.postMessage(msg);
            msg.remove("command");
            msg.add("command", "getFiles");
            msg.add("patterns", txtPatterns.Text);
            translater.postMessage(msg);
        }


        // -----< Functionality to take place when the publish button or publish menu item is single-left-clicked: Starts the conversion procedure >------------------------------------
        private void BtnPublish_OnClick(object sender, RoutedEventArgs e)
        {
            if (flag != 1)
            {
                Console.WriteLine("\n\t\t--> Clicked the Publish button in the GUI");
            }
            txtProgress.Text = "Converting";
            Auto_Publish();
            if (flag != 1)
            {
                Console.WriteLine("\n\t\t\t--> Converted the required files in the Server");
            }
        }

        //----< Clears the items in Converted Files Listbox >-------
        private void clearConvertedFilesList()
        {
            lstConverted.Items.Clear();
        }

        //----< Adds the items in Converted Files Listbox >-------
        private void addConvertedFilesList(string file)
        {
            lstNote.Text = "";
            int pos = file.LastIndexOf("\\");
            file = file.Substring(pos + 1, file.Length - pos - 1);
            lstConverted.Items.Add(file);
            if (flag != 1)
            {
                downloadFileButton.IsEnabled = true;
                downAllFilesButton.IsEnabled = true;
            }
        }

        // -----< Helper for loadConvertedFilesList, used to auto download and pop up for automatic test suite >------------------------------------
        private void AutoCallDownloadAndPopUp()
        {
            downloadFileButton.IsEnabled = true;
            downAllFilesButton.IsEnabled = true;
            popAllUpButton.IsEnabled = true;
            popUpButton.IsEnabled = true;
            Auto_Show_All();
            popAllFiles();
            NowDisplayingEvents();
            flag += 1;
        }

        // -----< To populate Converted Files Listbox, recieves list of converted files from server to populate >------------------------------------
        private void loadConvertedFilesList()
        {
            Action<CsMessage> setConvertedFilesList = (CsMessage rcvMsg) =>
            {
                Action clearConvertedFiles = () =>
                {
                    clearConvertedFilesList();
                };
                Dispatcher.Invoke(clearConvertedFiles, new Object[] { });

                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("convertedFile"))
                    {
                        Action<string> addConvertedFiles = (string file) =>
                        {
                            addConvertedFilesList(file);
                        };
                        Dispatcher.Invoke(addConvertedFiles, new Object[] { enumer.Current.Value });
                    }
                }
                Action AutoShow = () =>
                {
                    if (flag == 1)
                    {
                        if (lstConverted.Items.Count == 0)
                        {
                            lstNote.Text = "No files converted with your parameters!! Please try again with different parameters!!";
                            Console.WriteLine("\n\n\t--> No files converted with your parameters!! Please try again with different parameters!!");
                            txtProgress.Text = "No files converted with your parameters!! Please try again with different parameters!!";
                        }
                        else
                        {
                            AutoCallDownloadAndPopUp();
                        }
                    }
                    if (flag != 1 && lstConverted.Items.Count == 0)
                    {
                        Console.WriteLine("\n\n\t--> No files converted with your parameters!! Please try again with different parameters!!");
                        lstNote.Text = "No files converted with your parameters!! Please try again with different parameters!!";
                        txtProgress.Text = "No files converted with your parameters!! Please try again with different parameters!!";
                    }
                    };
                Dispatcher.Invoke(AutoShow, new Object[] { });
            };
            addClientProc("setConvertedFilesList", setConvertedFilesList);
        }

        // -----< Helper for popAllFiles, just inserts some lines on console >------------------------------------
        private void HelperInsert()
        {
            Console.WriteLine("\n\n\t--> Auto-Launching all the files in the Converted Files ListBox");
            Console.WriteLine("\t------------------------------------------------------------\n");
        }

        // -----< To pop all files mentioned in Converted Files ListBox >------------------------------------
        private void popAllFiles()
        {
            if (flag == 1)
            {
                HelperInsert();
            }
            else
            {
                Console.WriteLine("\n\t\t--> Clicked the Pop Up All Files button in the GUI");
            }
            lstConverted.SelectionMode = SelectionMode.Multiple;
            lstConverted.SelectAll();
            var selectedItems = lstConverted.SelectedItems;
            if (selectedItems.Count == 0)
            {
                Console.WriteLine("\t\t--> No files to pop up in the browser");
                lstNote.Text = "No files to pop up in the browser";
                txtProgress.Text = "No files to pop up in the browser";
                lstConverted.UnselectAll();
            }
            else
            {
                foreach (var selectedItem in selectedItems)
                {
                    if (!selectedItem.Equals(""))
                    {
                        var file = ConvertedWebPagesDirectory + "\\" + selectedItem;
                        if (File.Exists(file))
                        {
                            lstNote.Text = "";
                            lstNote.Text = "Opening all files " + Path.GetFileName(file) + " in the Browser";
                            txtProgress.Text = "Opening all files " + Path.GetFileName(file) + " in the Browser";
                            Console.WriteLine("\t\t--> Opening  all files {0} in the default browser", Path.GetFileName(file));
                            var demoThread = System.Diagnostics.Process.Start(file);
                            demoThread.WaitForExit();
                            if (demoThread.HasExited)
                            {
                                lstNote.Text = "Closed file " + Path.GetFileName(file) + " from the Browser";
                                txtProgress.Text = "Closed file " + Path.GetFileName(file) + " from the Browser";
                                Console.WriteLine("\t\t\t--> Closed {0} in the default browser", Path.GetFileName(file));
                            }
                        }
                    }
                }
                lstConverted.UnselectAll();
                lstNote.Text = "Popping up all files in the browser completed";
                txtProgress.Text = "Popping up all files in the browser completed";
            }
            lstConverted.SelectionMode = SelectionMode.Single;
        }

        // -----< Functionality to take place when Pop All Files button is clicked >------------------------------------
        private void PopAllUpButton_OnClick(object sender, RoutedEventArgs e)
        {
            popAllFiles();
        }

        // -----< To pop a file selected in Converted Files ListBox >------------------------------------
        private void popAFile()
        {
            Console.WriteLine("\n\t\t--> Clicked the Pop Up File button in the GUI");
            if (lstConverted.SelectedItem.ToString() != "" && lstConverted.SelectedItem != null)
            {
                var selectedItems = lstConverted.SelectedItem;
                var file = ConvertedWebPagesDirectory + "\\" + selectedItems;
                if (File.Exists(file))
                {
                    lstNote.Text = "";
                    lstNote.Text = "Opening file " + Path.GetFileName(file) + " in the Browser";
                    txtProgress.Text = "Opening file " + Path.GetFileName(file) + " in the Browser";
                    Console.WriteLine("\t\t--> Opening file {0} in the default browser", Path.GetFileName(file));
                    var demoThread = System.Diagnostics.Process.Start(file);
                    demoThread.WaitForExit();
                    if (demoThread.HasExited)
                    {
                        lstNote.Text = "Closed file " + Path.GetFileName(file) + " from the Browser";
                        txtProgress.Text = "Closed file " + Path.GetFileName(file) + " from the Browser";
                        Console.WriteLine("\t\t\t--> Closed {0} in the default browser", Path.GetFileName(file));
                    }
                }
                else
                {
                    lstConverted.SelectionMode = SelectionMode.Multiple;
                    lstConverted.SelectAll();
                    var items = lstConverted.SelectedItems;
                    foreach (var item in items)
                    {
                        var fileTemp = ConvertedWebPagesDirectory + "\\" + item;
                        if (File.Exists(fileTemp))
                        {
                            lstNote.Text = "Please select the file you downloaded to pop it up, i.e. " + item;
                            txtProgress.Text = "Please select the file you downloaded to pop it up, i.e. " + item;
                            lstConverted.UnselectAll();
                            lstConverted.SelectionMode = SelectionMode.Single;
                            break;
                        }
                    }

                }
            }
            else
            {
                Console.WriteLine("\t\t--> Please select a file to pop it up in the default browser!!");
                lstNote.Text = "Please select a file to pop it up in the default browser!!";
                txtProgress.Text = "Please select a file to pop it up in the default browser!!";
            }

        }

        // -----< Functionality to take place when Pop A File button is clicked >------------------------------------
        private void ButtonPopUp_OnClick(object sender, RoutedEventArgs e)
        {
            popAFile();
        }

        // -----< Functionality to take place when file is send from server, this is just to receive request and do nothing >------------------------------------
        private void recieveFilesFromServer()
        {
            Action<CsMessage> receiveConvertedFile = (CsMessage rcvMsg) =>
            {

            };
            addClientProc("receiveConvertedFile", receiveConvertedFile);
        }

        // -----< For downloading the files selected in Converted List Files ListBox from the server >------------------------------------
        private void autoViewFile()
        {
            Console.WriteLine("\n\t\t--> Clicked the Download File button in the GUI");
            string exactPath = Path.GetFullPath("../../../../ReceivedFilesFromServer");
            System.IO.DirectoryInfo di = new DirectoryInfo(exactPath);
            foreach (FileInfo file in di.GetFiles())
            {
                file.Delete();
            }
            foreach (DirectoryInfo dir in di.GetDirectories())
            {
                dir.Delete(true);
            }
            if (lstConverted.SelectedItem.ToString() != "" && lstConverted.SelectedItem != null)
            {
                if (lstConverted.SelectedItems.Count > 1 || flag == 1)
                {
                    foreach (var itemIn in lstConverted.SelectedItems)
                    {
                        CsEndPoint serverEndPoint = new CsEndPoint();
                        serverEndPoint.machineAddress = ServerUrl;
                        serverEndPoint.port = ServerPort;
                        txtProgress.Text = "Downloading file: " + itemIn + "!! Please wait!!";
                        lstNote.Text = "Downloading file: " + itemIn + "!! Please wait!!";
                        CsMessage msg = new CsMessage();
                        msg.add("to", CsEndPoint.toString(serverEndPoint));
                        msg.add("from", CsEndPoint.toString(endPoint_));
                        msg.add("command", "transferConvertedFiles");
                        msg.add("file", itemIn.ToString());
                        translater.postMessage(msg);
                    }
                    txtProgress.Text = "Downloading completed!!";
                    lstNote.Text = "Downloading completed!!";
                    popAllUpButton.IsEnabled = true;
                }
                else
                {
                    SingleFileDownloadHelper();
                }
            }
            else
            {
                if (flag != 1)
                {
                    Console.WriteLine("\t\t--> Please select file to download!!");
                    lstNote.Text = "Please select file to download!!";
                }
            }
        }

        // -----< Helper for autoViewFile to handle single selected file case >------------------------------------
        private void SingleFileDownloadHelper()
        {
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = ServerUrl;
            serverEndPoint.port = ServerPort;
            txtProgress.Text = "Downloading file: " + lstConverted.SelectedItem.ToString() + "!! Please wait!!";
            lstNote.Text = "Downloading file: " + lstConverted.SelectedItem.ToString() + "!! Please wait!!";
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "transferConvertedFiles");
            msg.add("file", lstConverted.SelectedItem.ToString());
            translater.postMessage(msg);
            popUpButton.IsEnabled = true;
            txtProgress.Text = "Downloading completed!!";
            lstNote.Text = "Downloading completed!!";
            popAllUpButton.IsEnabled = true;
        }

        // -----< Functionality to take place when Download A File button is clicked >------------------------------------
        private void ButtonViewFiles_OnClick(object sender, RoutedEventArgs e)
        {
            autoViewFile();
        }

        // -----< Helper for downloading the files selected in Converted List Files ListBox from the server >------------------------------------
        private void Auto_Show_All()
        {
            if (flag == 1)
            {
                Console.WriteLine("\n\n\t--> Auto-Launching all the files in the Converted Files ListBox");
                Console.WriteLine("\t------------------------------------------------------------\n");
            }
            else
            {
                Console.WriteLine("\n\t\t--> Clicked the Download All Files button in the GUI");
            }
            lstConverted.SelectionMode = SelectionMode.Multiple;
            lstConverted.SelectAll();
            autoViewFile();
            lstConverted.UnselectAll();
            lstConverted.SelectionMode = SelectionMode.Single;
        }

        // -----< Functionality to happen when Download All Files button is single-left-clicked >------------------------------------
        private void AllDisplayPop(object sender, RoutedEventArgs e)
        {
            Auto_Show_All();
        }

        // -----< Helper function for Auto_Publish, sends arguments and message to server >------------------------------------
        private void HelperExecuteCode(StringBuilder sendingMockArgs)
        {
            Console.WriteLine();
            string sendingArgs = sendingMockArgs.ToString();
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = ServerUrl;
            serverEndPoint.port = ServerPort;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getConvertedFiles");
            msg.add("cmdArgs", sendingArgs);
            translater.postMessage(msg);
            ConvertedList.IsEnabled = true;
            lstConverted.Items.Clear();
            downloadFileButton.IsEnabled = false;
            downAllFilesButton.IsEnabled = false;
            popAllUpButton.IsEnabled = false;
            popUpButton.IsEnabled = false;
            lstConvert.Text = "";
            ConvertedList.IsSelected = true;
            lstNote.Text = "Converting...!! Please wait for the list to be populated!!";
            txtProgress.Text = "Converted Files View";
        }

        // -----< Functionality to auto publish when window is loaded and also the overall publishing functionality >------------------------------------
        private void Auto_Publish()
        {
            if (flag == 1)
            {
                Console.WriteLine("\n\n\t--> Auto-Publishing with the default values now");
                Console.WriteLine("\t------------------------------------------------\n");
                Console.WriteLine("\t\t--> Clicked the Publish button in the GUI");
            }
            else
            {
                Console.WriteLine("\t\t--> Clicked the Publish button in the GUI");
            }
            txtProgress.Text = "Converting";
            List<string> args = new List<string>();
            string toAnalyzeDirectory = string.Empty;
            args.Add("RemoteCodePageManagement.exe");
            if (txtPath.Text == "")
            {
                toAnalyzeDirectory = "..";
            }
            else
            {
                toAnalyzeDirectory = "../" + removeFirstDir(txtPath.Text);
            }
            args.Add(toAnalyzeDirectory);
            if (cbRecurse.IsChecked.Value)
            {
                args.Add("/s");
            }
            args.AddRange(Patterns.Split(' '));
            args.AddRange(Regex.Split(' '));
            StringBuilder sendingMockArgs = new StringBuilder();
            Console.WriteLine("\t\t--> The command line arguments to send are:");
            Console.Write("\t\t\t--> ");
            foreach (var arg in args)
            {
                Console.Write(arg + " ");
                sendingMockArgs.Append(arg + " ");
            }
            HelperExecuteCode(sendingMockArgs);
        }

        // -----< Functionality to update the patterns when a key up occurs in the text box and finalize the change on enter key press >------------------------------------
        private void TxtPatterns_OnKeyUp(object sender, KeyEventArgs e)
        {
            Console.WriteLine("\t\t--> Trying to change the Patterns right now");
            if (e.Key == Key.Enter)
            {
                Patterns = txtPatterns.Text;
                txtProgress.Text = "The new Patterns are: " + Patterns;
                Console.WriteLine("\t\t\t--> Successful in changing the patterns");
                Console.WriteLine("\t\t\t--> The new Patterns are: {0}", Patterns);
                CsEndPoint serverEndPoint = new CsEndPoint();
                serverEndPoint.machineAddress = ServerUrl;
                serverEndPoint.port = ServerPort;
                CsMessage msg = new CsMessage();
                msg.add("to", CsEndPoint.toString(serverEndPoint));
                msg.add("from", CsEndPoint.toString(endPoint_));
                msg.add("command", "getDirs");
                msg.add("path", pathStack_.Peek());
                translater.postMessage(msg);
                msg.remove("command");
                msg.add("command", "getFiles");
                msg.add("patterns", txtPatterns.Text);
                translater.postMessage(msg);
            }
            txtProgress.Text = "The new Patterns are: " + Patterns;
        }

        // -----< Functionality to console print for letting the user know that the events following are the one's being triggered by the GUI >------------------------------------
        private void NowDisplayingEvents()
        {
            Console.WriteLine("\n----------------------------------------------------------------------------------------------------");
            Console.WriteLine("-----------------------< Now will display any event triggered in the GUI >-----------------------");
            Console.WriteLine("----------------------------------------------------------------------------------------------------\n");
        }

        // -----< Functionality to take place when Exit menu item is single-left-clicked: The GUI is closed on clicking >------------------------------------
        private void MenuItem_OnClick(object sender, RoutedEventArgs e)
        {
            txtProgress.Text = "Closing Code Publisher Client Now!!";
            Console.WriteLine("\t\t--> Closing Code Publisher Client Now!!");
            Console.WriteLine("\t\t\t--> Closed the Code Publisher Client!!\n");
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = ServerUrl;
            serverEndPoint.port = ServerPort;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "serverQuit");
            translater.postMessage(msg);
            Close();
        }

        // -----< Functionality to take place when the close button is clicked: The GUI is closed on clicking >------------------------------------
        void MainWindow_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = ServerUrl;
            serverEndPoint.port = ServerPort;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "serverQuit");
            translater.postMessage(msg);
            Console.WriteLine("\t\t--> Closing Code Publisher Client Now!!");
            Console.WriteLine("\t\t\t--> Closed the Code Publisher Client!!\n");
        }

        // -----< Functionality to update the regex when a key up occurs in the text box and finalize the change on enter key press >------------------------------------
        private void TxtRegex_OnKeyUp(object sender, KeyEventArgs e)
        {
            Console.WriteLine("\t\t--> Trying to change the Regex right now");
            if (e.Key == Key.Enter)
            {
                Regex = txtRegex.Text;
                txtProgress.Text = "The new Regex are: " + Regex;
                Console.WriteLine("\t\t\t--> Successful in changing the Regex");
                Console.WriteLine("\t\t\t--> The new Regex are: {0}", Regex);
            }
        }

        // -----< Helper for MenuItemFunctionality_OnClick, gives the list of strings to be added >------------------------------------
        private List<string> returnFunctionalityString()
        {
            return new List<string>
            {
                "1. \t You can connect the client and server by clicking on the Connect button. \n",
                "2. \t Click the sub-directories checkbox to enable recursive search of files from the current directory i.e. the path mentioned. \n",
                "3. \t Double-Left-Click in the directories mentioned in Navigation Tab to open the selected folder in Navigation View. \n",
                "4. \t Click the Publish button to convert the files matching the patterns and regex. \n",
                "5. \t Click the Converted Files tab to view the list of files converted after analysis. \n",
                "6. \t Single-Left-Click the Download A File Button to download the selected file. \n",
                "7. \t Single-Left-Click the Pop A File Button to pop the selected file in the default browser. \n",
                "8. \t Single-Left-Click the File Menu-Item to open drop down menu.\n",
                "9. \t Single-Left-Click Publish Menu-Item to convert the files matching the patterns and regex. \n",
                "10. \t Single-Left-Click Exit to exit the application. \n",
                "11. \t Single-Left-Click the Info Menu-Item to open drop down menu.\n",
                "12. \t Single-Left-Click the Functionality Menu-Item to open Code Pop Up Window.\n",
                "13. \tAfter typing a new pattern, press enter key to register the change\n",
                "14. \tAfter typing a new regex, press enter key to register the change\n",
                "15. \tClick the Download All Files Button to download all the files from the server mentioned in the converted files list box.\n",
                "16. \tClick the Pop All Files Button to display all the files one by one. Note: If the default browser is already opened, the files will open simultaneously. Also, the next file will only open after the current file is closed.\n",
            };
        }

        // -----< Functionality to pop up a window that mentions the functionalities of the GUI >------------------------------------
        private void MenuItemFunctionality_OnClick(object sender, RoutedEventArgs e)
        {
            Console.WriteLine("\t\t--> Clicked on Functionality menu item");
            try
            {
                var fileName = "Functionality.txt";
                var path = Path.Combine(WorkingDirectory, fileName);
                if (File.Exists(path))
                {
                    File.Delete(path);
                }

                var temp = returnFunctionalityString();
                using (TextWriter tw = new StreamWriter(path))
                {
                    foreach (var s in temp)
                    {
                        tw.WriteLine(s);
                    }
                }
                using (var sr = File.OpenText(path))
                {
                    Console.WriteLine("\t\t\t--> Opened Functionality.txt");
                    var contents = File.ReadAllText(path);
                    var popup = new CodePopUp();
                    popup.codeView.Text = contents;
                    popup.Show();
                }
                if (File.Exists(path))
                {
                    File.Delete(path);
                }
            }
            catch (Exception ex)
            {
                var msg = ex.Message;
                Console.WriteLine("\t\t\t--> Unable to open Functionality.txt");
            }
        }

        // -----< Code to demonstrate Requirement-One >------------------------------------
        private void DemoReqOne()
        {
            Console.WriteLine("\n-----------------------------------------------------------------");
            Console.WriteLine("-----------------------< Requirement-One >-----------------------");
            Console.WriteLine("-----------------------------------------------------------------");
            Console.WriteLine("\n  We are using Visual Studio 2017 and its C++ Windows Console Projects, as provided in the ECS computer labs");
            Console.WriteLine("\n-----------------------------------------------------------------");
            var directories = Directory.GetDirectories(WorkingDirectory);
            Console.WriteLine();
            foreach (var directory in directories)
            {
                Console.WriteLine("\t--> " + directory);
            }
        }

        // -----< Code to demonstrate Requirement-Two >------------------------------------
        private void DemoReqTwo()
        {
            Console.WriteLine("\n-----------------------------------------------------------------");
            Console.WriteLine("-----------------------< Requirement-Two >-----------------------");
            Console.WriteLine("-----------------------------------------------------------------");
            Console.WriteLine("\n  We are using Windows Presentation Foundation (WPF) and C# to implement the Client GUI i.e. Client Package");
            Console.WriteLine("\n-----------------------------------------------------------------");

            var directories = Directory.GetDirectories(WorkingDirectory);
            Console.WriteLine();
            foreach (var directory in directories)
            {
                string tempVar = Path.GetFileName(directory);
                if (tempVar == "Client")
                {
                    Console.WriteLine("\t--> " + directory);
                }
            }
        }

        // -----< Code to demonstrate Requirement-Three >------------------------------------
        private void DemoReqThree()
        {
            Console.WriteLine("\n-----------------------------------------------------------------");
            Console.WriteLine("-----------------------< Requirement-Three >---------------------");
            Console.WriteLine("-----------------------------------------------------------------");
            Console.WriteLine("\n  We have assembled working parts from Projects #1, #2, and #3 into a Client-Server configuration." +
            "\n  Unlike conventional Client-Server architectures, the Client need not wait for a reply from" +
            " the Publisher server\n for a request before sending additional requests.");
            Console.WriteLine("\n-----------------------------------------------------------------");

            var directories = Directory.GetDirectories(WorkingDirectory);
            Console.WriteLine();
            foreach (var directory in directories)
            {
                Console.WriteLine("\t--> " + directory);
            }
        }

        // -----< Code to demonstrate Requirement-Four >------------------------------------
        private void DemoReqFour()
        {
            Console.WriteLine("\n-----------------------------------------------------------------");
            Console.WriteLine("-----------------------< Requirement-Four >----------------------");
            Console.WriteLine("-----------------------------------------------------------------");
            Console.WriteLine("\n We have provided a Graphical User Interface (GUI) i.e. Client Package for the client that supports navigating remote " +
            "directories to find a project for conversion,\n and supports displaying the conversion results in a way that meets Project #3 requirements.");
            Console.WriteLine("\n-----------------------------------------------------------------");

            string clientPath = WorkingDirectory + "Client";
            var files = Directory.GetFiles(clientPath);
            Console.WriteLine();
            foreach (var file in files)
            {
                Console.WriteLine("\t--> " + file);
            }
        }

        // -----< Code to demonstrate Requirement-Five >------------------------------------
        private void DemoReqFive()
        {
            Console.WriteLine("\n-----------------------------------------------------------------");
            Console.WriteLine("-----------------------< Requirement-Five >----------------------");
            Console.WriteLine("-----------------------------------------------------------------");
            Console.WriteLine("\n We have provided message designs appropriate for this application. All messages are instances of the same Message" +
            " class, but have a specified set of attributes and body contents suited for the intended task.");
            Console.WriteLine("\n-----------------------------------------------------------------");

            Console.WriteLine();
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = ServerUrl;
            serverEndPoint.port = ServerPort;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "echo");
            translater.postMessage(msg);
            msg.show();
        }

        // -----< Code to demonstrate Requirement-Six, Seven and Eight >-----------------------------------
        private void DemoReqSix()
        {
            Console.WriteLine("\n-----------------------------------------------------------------");
            Console.WriteLine("-----------------------< Requirement-Six, Seven and Eight >-----------------------");
            Console.WriteLine("-----------------------------------------------------------------");
            Console.WriteLine("\n Requirement Six: We support converting source code in the server and, with a separate request," +
            " transferring one or more converted files back to the local client,\n using the communication channel. This is done by Publish button and Download File or Download All Files button respectively" +
            "\n\n Requirement Seven: We would be demonstrating correct operations for two or more concurrent clients below via the Automatic Test Suite." +
            "\n\n Requirement Eight: We have included an automated test that accepts the server url on its command line," +
            " invokes the Code Publisher,\n through its interface, to convert all the project files matching a pattern that accepts *.h and *.cpp files,\n and then opens the Client GUI's Display view." +
            " This is demonstrated below via Automatic Test Suite.\n");
        }

        // -----< Functionality to happen when Navigation Tab gets focus: Updates the status bar >------------------------------------
        private void NavigationTab_OnGotFocus(object sender, RoutedEventArgs e)
        {
            txtProgress.Text = "Navigation View";
        }

        // -----< Functionality to happen when Converted List Tab gets focus: Updates the status bar >------------------------------------
        private void ConvertedList_OnGotFocus(object sender, RoutedEventArgs e)
        {
            txtProgress.Text = "Converted Files View";
        }

        // -----< Functionality to happen when mouse enters File Menu Item element's area: Updates the status bar >------------------------------------
        private void FileMenuItem(object sender, MouseEventArgs e)
        {
            txtProgress.Text = "File Menu Item";
        }

        // -----< Functionality to happen when File Menu Item gets focus: Updates the status bar >------------------------------------
        private void FileItemFocus(object sender, RoutedEventArgs e)
        {
            txtProgress.Text = "File Menu Item";
        }

        // -----< Functionality to happen when the mouse leaves the GUI element's area: Clears the status bar >------------------------------------
        private void NewMouseLeave(object sender, MouseEventArgs e)
        {
            txtProgress.Text = "";
        }

        // -----< Functionality to happen when mouse enters Connect Button element's area: Updates the status bar >------------------------------------
        private void ConnectEnter(object sender, MouseEventArgs e)
        {
            txtProgress.Text = "Connect: Click to Connect Client and Server";
        }

        // -----< Functionality to happen when mouse enters Connect Button element's area: Updates the status bar >------------------------------------
        private void ConnectEnterFocus(object sender, RoutedEventArgs e)
        {
            txtProgress.Text = "Connect: Click to Connect Client and Server";
        }

        // -----< Functionality to happen when mouse enters Publish Menu Item or Button element's area: Updates the status bar >------------------------------------
        private void PublishEnter(object sender, MouseEventArgs e)
        {
            txtProgress.Text = "Publish: Click to Convert Files";
        }

        // -----< Functionality to happen when Publish Menu Item or Button gets focus: Updates the status bar >------------------------------------
        private void PublishEnterFocus(object sender, RoutedEventArgs e)
        {
            txtProgress.Text = "Publish: Click to Convert Files";
        }

        // -----< Functionality to happen when mouse enters Exit Menu Item element's area: Updates the status bar >------------------------------------
        private void ExitEnter(object sender, MouseEventArgs e)
        {
            txtProgress.Text = "Exit";
        }

        // -----< Functionality to happen when Exit Menu Item gets focus: Updates the status bar >------------------------------------
        private void ExitEnterFocus(object sender, RoutedEventArgs e)
        {
            txtProgress.Text = "Exit";
        }

        // -----< Functionality to happen when mouse enters Functionality Menu Item element's area: Updates the status bar >------------------------------------
        private void FuncEnter(object sender, MouseEventArgs e)
        {
            txtProgress.Text = "Functionality";
        }

        // -----< Functionality to happen when Functionality Menu Item gets focus: Updates the status bar >------------------------------------
        private void FuncEnterFocus(object sender, RoutedEventArgs e)
        {
            txtProgress.Text = "Functionality";
        }

        // -----< Functionality to happen when mouse enters Information Menu Item element's area: Updates the status bar >------------------------------------
        private void InfoEnter(object sender, MouseEventArgs e)
        {
            txtProgress.Text = "Information";
        }

        // -----< Functionality to happen when Information Menu Item gets focus: Updates the status bar >------------------------------------
        private void InfoEnterFocus(object sender, RoutedEventArgs e)
        {
            txtProgress.Text = "Information";
        }

        // -----< Functionality to happen when mouse enters Path label element's area: Updates the status bar >------------------------------------
        private void PathEnter(object sender, MouseEventArgs e)
        {
            txtProgress.Text = "Path is: " + txtPath.Text;
        }

        // -----< Functionality to happen when Path label gets focus: Updates the status bar >------------------------------------
        private void PathEnterFocus(object sender, RoutedEventArgs e)
        {
            txtProgress.Text = "Path is: " + txtPath.Text;
        }

        // -----< Functionality to happen when mouse enters Sub-Driectories label element's area: Updates the status bar >------------------------------------
        private void SubDirEnter(object sender, MouseEventArgs e)
        {
            txtProgress.Text = "Choose Recursive Sub-Directories Search";
        }

        // -----< Functionality to happen when Sub-Directories label gets focus: Updates the status bar >------------------------------------
        private void SubDirEnterFocus(object sender, RoutedEventArgs e)
        {
            txtProgress.Text = "Choose Recursive Sub-Directories Search";
        }

        // -----< Functionality to happen when mouse enters Navigation Tab element's area: Updates the status bar >------------------------------------
        private void NavigationTab_OnMouseEnter(object sender, MouseEventArgs e)
        {
            txtProgress.Text = "Navigation View";
        }

        // -----< Functionality to happen when mouse enters Navigation List element's area: Updates the status bar >------------------------------------
        private void LstFiles_OnMouseEnter(object sender, MouseEventArgs e)
        {
            txtProgress.Text = "Navigation List";
        }

        // -----< Functionality to happen when Navigation List gets focus: Updates the status bar >------------------------------------
        private void LstFiles_OnGotFocus(object sender, RoutedEventArgs e)
        {
            txtProgress.Text = "Navigation List";
        }

        // -----< Functionality to happen when mouse enters Sub-Directories checkbox element's area: Updates the status bar >------------------------------------
        private void CbRecurse_OnMouseEnter(object sender, MouseEventArgs e)
        {
            if (cbRecurse.IsChecked.Value)
            {
                txtProgress.Text = "Recursive Folder Search Checked as: True";
            }
            else
            {
                txtProgress.Text = "Recursive Folder Search Checked as: False";
            }
        }

        // -----< Functionality to happen when mouse click on Sub-Directories checkbox element's area: Updates the status bar >------------------------------------
        private void CbRecurse_OnClick(object sender, RoutedEventArgs e)
        {
            if (cbRecurse.IsChecked.Value)
            {
                txtProgress.Text = "Recursive Folder Search Now Checked as: True";
            }
            else
            {
                txtProgress.Text = "Recursive Folder Search Now Checked as: False";
            }
        }

        // -----< Functionality to happen when mouse enters Patterns label element's area: Updates the status bar >------------------------------------
        private void PatternsEnter(object sender, MouseEventArgs e)
        {
            txtProgress.Text = "Patterns";
        }

        // -----< Functionality to happen when Patterns label gets focus: Updates the status bar >------------------------------------
        private void PatternsEnterFocus(object sender, RoutedEventArgs e)
        {
            txtProgress.Text = "Patterns";
        }

        // -----< Functionality to happen when mouse enters Patterns text box element's area: Updates the status bar >------------------------------------
        private void TxtPatterns_OnMouseEnter(object sender, MouseEventArgs e)
        {
            txtProgress.Text = "Current patterns: " + txtPatterns.Text;
        }

        // -----< Functionality to happen when Patterns text box gets focus: Updates the status bar >------------------------------------
        private void TxtPatterns_OnGotFocus(object sender, RoutedEventArgs e)
        {
            txtProgress.Text = "Current patterns: " + txtPatterns.Text;
        }

        // -----< Functionality to happen when mouse click on Patterns text box element's area: Updates the status bar >------------------------------------
        private void TxtPatterns_OnMouseUp(object sender, MouseButtonEventArgs e)
        {
            txtProgress.Text = "Current patterns: " + txtPatterns.Text;
        }

        // -----< Functionality to happen when mouse enters Regex text box element's area: Updates the status bar >------------------------------------
        private void TxtRegex_OnMouseEnter(object sender, MouseEventArgs e)
        {
            txtProgress.Text = "Current regex: " + txtRegex.Text;
        }

        // -----< Functionality to happen when Regex label gets focus: Updates the status bar >------------------------------------
        private void TxtRegex_OnGotFocus(object sender, RoutedEventArgs e)
        {
            txtProgress.Text = "Current regex: " + txtRegex.Text;
        }

        // -----< Functionality to happen when mouse click on Regex text box element's area: Updates the status bar >------------------------------------
        private void TxtRegex_OnMouseUp(object sender, MouseButtonEventArgs e)
        {
            txtProgress.Text = "Current regex: " + txtRegex.Text;
        }

        // -----< Functionality to happen when mouse enters Regex label element's area: Updates the status bar >------------------------------------
        private void Regex_OnMouseEnter(object sender, MouseEventArgs e)
        {
            txtProgress.Text = "Regex";
        }

        // -----< Functionality to happen when Regex text box gets focus: Updates the status bar >------------------------------------
        private void Regex_OnGotFocus(object sender, RoutedEventArgs e)
        {
            txtProgress.Text = "Regex";
        }

        // -----< Functionality to happen when mouse enters Display All Items button element's area: Updates the status bar >------------------------------------
        private void AllDisplayPopEnter(object sender, MouseEventArgs e)
        {
            txtProgress.Text = "Click to display all of the converted files on the browser, one by one";
        }

        // -----< Functionality to happen when Display All Files button gets focus: Updates the status bar >------------------------------------
        private void AllDisplayPopEnterFocus(object sender, RoutedEventArgs e)
        {
            txtProgress.Text = "Click to display all of the converted files on the browser, one by one";
        }

        // -----< Functionality to happen when single-left-clicked an element in Converted Files ListBox >------------------------------------
        private void LstConverted_OnMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            lstNote.Text = "";
            if (lstConverted.SelectedItem != null)
            {
                lstNote.Text = "";
                lstConvert.Text = lstConverted.SelectedItem.ToString();
                selectedFile = lstConvert.Text;
            }
        }

        // -----< Functionality to happen when mouse enters Converted File Textbox element's area: Updates the status bar >------------------------------------
        private void convTextEnter(object sender, MouseEventArgs e)
        {
            if (lstConverted.SelectedItem != null)
            {
                txtProgress.Text = "Selected File: " + lstConverted.SelectedItem.ToString();
            }
        }

        // -----< Functionality to happen when Converted File Textbox element's area gets focus: Updates the status bar >------------------------------------
        private void convTextEnterFocus(object sender, RoutedEventArgs e)
        {
            if (lstConverted.SelectedItem != null)
            {
                txtProgress.Text = "Selected File: " + lstConverted.SelectedItem.ToString();
            }
        }

        // -----< Functionality to happen when mouse enters Download A File button element's area: Updates the status bar >------------------------------------
        private void downFileEnter(object sender, MouseEventArgs e)
        {
            if (lstConverted.SelectedItem != null)
            {
                txtProgress.Text = "Downloading File: " + lstConverted.SelectedItem.ToString();
            }
        }

        // -----< Functionality to happen when Download A File button element's gets focus: Updates the status bar >------------------------------------
        private void downFileEnterFocus(object sender, RoutedEventArgs e)
        {
            if (lstConverted.SelectedItem != null)
            {
                txtProgress.Text = "Downloading File: " + lstConverted.SelectedItem.ToString();
            }
        }

        // -----< Functionality to happen when mouse enters Pop A File button element's area: Updates the status bar >------------------------------------
        private void popAFileEnter(object sender, MouseEventArgs e)
        {
            if (lstConverted.SelectedItem != null)
            {
                txtProgress.Text = "Pop Up File: " + lstConverted.SelectedItem.ToString();
            }
        }

        // -----< Functionality to happen when Pop A File button element's gets focus: Updates the status bar >------------------------------------
        private void popAFileEnterFocus(object sender, RoutedEventArgs e)
        {
            if (lstConverted.SelectedItem != null)
            {
                txtProgress.Text = "Pop Up File: " + lstConverted.SelectedItem.ToString();
            }
        }

        // -----< Functionality to happen when mouse enters Download All Files button element's area: Updates the status bar >------------------------------------
        private void downAllFileEnter(object sender, MouseEventArgs e)
        {
            txtProgress.Text = "Click to download all files in the listbox";
        }

        // -----< Functionality to happen when Download All Files button element's gets focus: Updates the status bar >------------------------------------
        private void downAllFileEnterFocus(object sender, RoutedEventArgs e)
        {
            txtProgress.Text = "Click to download all files in the listbox";
        }

        // -----< Functionality to happen when NOTE element's gets entered: Updates the status bar >------------------------------------
        private void noteEnter(object sender, MouseEventArgs e)
        {
            txtProgress.Text = "Important information is displayed here";
        }

        // -----< Functionality to happen when NOTE element's gets focus: Updates the status bar >------------------------------------
        private void noteEnterFocus(object sender, RoutedEventArgs e)
        {
            txtProgress.Text = "Important information is displayed here";
        }

        // -----< Functionality to happen when Client Port element's gets entered: Updates the status bar >------------------------------------
        private void clientEnter(object sender, MouseEventArgs e)
        {
            txtProgress.Text = "Client URL: " + txtClientPort.Text;
        }

        // -----< Functionality to happen when Client Port element's gets focus: Updates the status bar >------------------------------------
        private void clientEnterFocus(object sender, RoutedEventArgs e)
        {
            txtProgress.Text = "Client URL: " + txtClientPort.Text;
        }

        // -----< Functionality to happen when Server Port element's gets entered: Updates the status bar >------------------------------------
        private void serverEnter(object sender, MouseEventArgs e)
        {
            txtProgress.Text = "Server URL: " + txtServerPort.Text;
        }

        // -----< Functionality to happen when Server Port element's gets focus: Updates the status bar >------------------------------------
        private void serverEnterFocus(object sender, RoutedEventArgs e)
        {
            txtProgress.Text = "Server URL: " + txtServerPort.Text;
        }
    }
}