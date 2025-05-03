#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
using namespace std;

const string ClientsFileName = "Clients.txt";
const string UsersFileName = "Users.txt";

short Permissions = 0;

enum enMainMenueOptions { eListClients = 1, eAddNewClient = 2, eDeleteClient = 3, eUpdateClient = 4, eFindClient = 5, eShowTransactionsMenue= 6, eManageUsers= 7, eLogout = 8 };
enum enTransactionsMenueOptions { eDeposit = 1, eWithdraw = 2, eShowTotalBalance = 3, eShowMainMenue = 4 };
enum enManageUsersMenueOptions { eListUsers = 1, eAddUser = 2, eDeleteUser = 3, eUpdateUser = 4, eFindUser = 5, eMainMenue = 6 };
enum enPermissions {eDefault=0, eListClients_ = 1 << 0, eAddNewClient_ = 1 << 1, eDeleteClient_ = 1 << 2, eUpdateClient_ = 1 << 3, eFindClient_ = 1 << 4, eShowTransactionsMenue_ = 1 << 5, eManageUsers_ = 1 << 6 ,eAll=-1};

void ShowMainMenue();
void ShowTransactionsMenue();
void ShowManageUsersMenueScreen();
void Login();

struct sClient
{
    string AccountNumber = "";
    string PinCode = "";
    string Name = "";
    string Phone = "";
    double AccountBalance = 0.0;
    bool MarkForDelete = false;
};

struct sUser
{
    string UserName = "";
    string Password = "";
    short Permissions;
    bool MarkForDelete = false;
};

vector<string> SplitString(string S1, string Delim)
{
    vector<string> vString;
    short pos = 0;
    string sWord;
 
    while ((pos = S1.find(Delim)) != std::string::npos)
    {
        sWord = S1.substr(0, pos);   
        if (sWord != "")
        {
            vString.push_back(sWord);
        }

        S1.erase(0, pos + Delim.length());  
    }

    if (S1 != "")
    {
        vString.push_back(S1);
    }

    return vString;

}

sClient ConvertLinetoRecord_Clients(string Line, string Seperator = "#//#")
{
    sClient Client;
    vector<string> vClientData;
    vClientData = SplitString(Line, Seperator);

    Client.AccountNumber = vClientData[0];
    Client.PinCode = vClientData[1];
    Client.Name = vClientData[2];
    Client.Phone = vClientData[3];
    Client.AccountBalance = stod(vClientData[4]);
    return Client;
}

sUser ConvertLinetoRecord_Users(string Line, string Seperator = "#//#")
{
    sUser User;
    vector<string>vUserData ;
    vUserData = SplitString(Line, Seperator);

    User.UserName = vUserData[0];
    User.Password = vUserData[1];
    User.Permissions = stoi(vUserData[2]);
    return User;
}

string ConvertRecordToLine_Clients(sClient Client, string Seperator ="#//#")
{
    string stClientRecord = "";
    stClientRecord += Client.AccountNumber + Seperator;
    stClientRecord += Client.PinCode + Seperator;
    stClientRecord += Client.Name + Seperator;
    stClientRecord += Client.Phone + Seperator;
    stClientRecord += to_string(Client.AccountBalance);
    return stClientRecord;
}

string ConvertRecordToLine_Users(sUser User, string Seperator = "#//#")
{
    string stClientRecord = "";
    stClientRecord += User.UserName + Seperator;
    stClientRecord += User.Password + Seperator;
    stClientRecord += to_string(User.Permissions);
    return stClientRecord;
}

bool ClientExistsByAccountNumber(string AccountNumber, string FileName)
{
     fstream MyFile;
    MyFile.open(FileName, ios::in); 

    if (MyFile.is_open())
    {
        string Line;
        sClient Client;

        while (getline(MyFile, Line))
        {
            Client = ConvertLinetoRecord_Clients(Line);
            if (Client.AccountNumber == AccountNumber)
            {
                MyFile.close();
                return true;
            }
         }
        MyFile.close();
    }
    return false;
}

bool UserExistByUserName(string UserName, string FileName)
{
    fstream MyFile;
    MyFile.open(FileName, ios::in);

    if (MyFile.is_open())
    {
        string Line;
        sUser User;

        while (getline(MyFile, Line))
        {
            User = ConvertLinetoRecord_Users(Line);
            if (User.UserName == UserName)
            {
                MyFile.close();
                return true;
            }
        }
        MyFile.close();
        return false;
    }
}

sClient ReadNewClient()
{
    sClient Client;
    cout << "Enter Account Number? ";
    getline(cin >> ws, Client.AccountNumber);

    while (ClientExistsByAccountNumber(Client.AccountNumber, ClientsFileName))
    {
        cout << "\nClient with [" << Client.AccountNumber << "] already exists, Enter another Account Number? ";
        getline(cin >> ws, Client.AccountNumber);
    }

    cout << "Enter PinCode? ";
    getline(cin, Client.PinCode);

    cout << "Enter Name? ";
    getline(cin, Client.Name);

    cout << "Enter Phone? ";
    getline(cin, Client.Phone);

    cout << "Enter AccountBalance? ";
    cin >> Client.AccountBalance;

    return Client;
}

enPermissions GiveAccessToPermissions()
{
    char Answer_Yes_No;

    cout << "\nDo you want to give full access? y/n? ";
    cin >> Answer_Yes_No;
    
    if (toupper(Answer_Yes_No)=='Y')
    {
        return enPermissions::eAll;
    }

    struct sPermissionsOption
    {
        enPermissions Permission;
        string Description;
    };

    enPermissions ValueOfPermissionsInBits = eDefault;
    vector<sPermissionsOption>vPermissionsOption = {
        {enPermissions::eListClients_,"Show Client List? y/n? "},
        {enPermissions::eAddNewClient_,"\nAdd New Client? y/n? " },
        {enPermissions::eDeleteClient_,"\nDelete Client? y/n/ "},
        {enPermissions::eUpdateClient_,"\nUpdate Client? y/n? "},
        {enPermissions::eFindClient_,"\nFind Client? y/n? "},
        {enPermissions::eShowTransactionsMenue_,"\nTransactions? y/n? "},
        {enPermissions::eManageUsers_,"\nManage Users? y/n? "}
    };

    cout << "\nDo you want to give access to : \n\n";
    for (const auto& Option : vPermissionsOption)
    {
        cout << Option.Description;
        cin >> Answer_Yes_No;

        if (toupper(Answer_Yes_No) == 'Y')
        {
            ValueOfPermissionsInBits = static_cast<enPermissions>(ValueOfPermissionsInBits | Option.Permission);
        }
    }
    if (ValueOfPermissionsInBits == 127)
        return enPermissions::eAll;
    else
        return ValueOfPermissionsInBits;
}

sUser ReadNewUser()
{
    sUser User;
    char Answer_Yes_No;
    cout << "Enter Username? ";
    getline(cin >> ws, User.UserName);

    while (UserExistByUserName(User.UserName, UsersFileName))
    {
        cout << "\nUser with [" << User.UserName << "] already exists, Enter another username? ";
        getline(cin >> ws, User.UserName);
    }

    cout << "Enter Password? ";
    getline(cin, User.Password);

    User.Permissions = static_cast<short>(GiveAccessToPermissions());
    return User;
}

vector <sClient> LoadClientsDataFromFile(string FileName)
{
    vector <sClient> vClients;
    fstream MyFile;
    MyFile.open(FileName, ios::in);

    if (MyFile.is_open())
    {
        string Line;
        sClient Client;

        while (getline(MyFile, Line))
        {
            Client = ConvertLinetoRecord_Clients(Line);
            vClients.push_back(Client);
        }
        MyFile.close();
    }
    return vClients;
}

vector <sUser> LoadUsersDataFromFile(string FileName)
{
    vector <sUser> vUsers;
    fstream MyFile;
    MyFile.open(FileName, ios::in);

    if (MyFile.is_open())
    {
        string Line;
        sUser User;

        while (getline(MyFile, Line))
        {
            User = ConvertLinetoRecord_Users(Line);
            vUsers.push_back(User);
        }
        MyFile.close();
    }
    return vUsers;
}

void PrintClientRecordLine(sClient Client)
{
    cout << "| " << setw(15) << left << Client.AccountNumber;
    cout << "| " << setw(10) << left << Client.PinCode;
    cout << "| " << setw(40) << left << Client.Name;
    cout << "| " << setw(12) << left << Client.Phone;
    cout << "| " << setw(12) << left << Client.AccountBalance;
}

void PrintUserRecordLine(sUser User)
{
    cout << "| " << setw(15) << left << User.UserName;
    cout << "| " << setw(12) << left << User.Password;
    cout << "| " << setw(12) << left << User.Permissions << endl;
 }

void ShowAllClientsScreen()
{
    vector <sClient> vClients = LoadClientsDataFromFile(ClientsFileName);

    cout << "\n\t\t\t\t\tClient List (" << vClients.size() << ") Client(s).";
    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;

    cout << "| " << left << setw(15) << "Accout Number";
    cout << "| " << left << setw(10) << "Pin Code";
    cout << "| " << left << setw(40) << "Client Name";
    cout << "| " << left << setw(12) << "Phone";
    cout << "| " << left << setw(12) << "Balance";
    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;

    if (vClients.size() == 0)
        cout << "\t\t\t\tNo Clients Available In the System!";
    else
        for (sClient Client : vClients)
        {

            PrintClientRecordLine(Client);
            cout << endl;
        }

    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;
}

void ShowTotalBalances()
{

    vector <sClient> vClients = LoadClientsDataFromFile(ClientsFileName);

    cout << "\n\t\t\t\t\tBalances List (" << vClients.size() << ") Client(s).";
    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;
    cout << "| " << left << setw(15) << "Accout Number";
    cout << "| " << left << setw(40) << "Client Name";
    cout << "| " << left << setw(12) << "Balance";
    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;

    double TotalBalances = 0;

    if (vClients.size() == 0)
        cout << "\t\t\t\tNo Clients Available In the System!";
    else

        for (sClient Client : vClients)
        {

            PrintClientRecordLine(Client);
            TotalBalances += Client.AccountBalance;

            cout << endl;
        }

    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;
    cout << "\t\t\t\t\t   Total Balances = " << TotalBalances;

}

void PrintClientCard(sClient Client)
{
    cout << "\nThe following are the client details:\n";
    cout << "-----------------------------------";
    cout << "\nAccout Number: " << Client.AccountNumber;
    cout << "\nPin Code     : " << Client.PinCode;
    cout << "\nName         : " << Client.Name;
    cout << "\nPhone        : " << Client.Phone;
    cout << "\nAccount Balance: " << Client.AccountBalance;
    cout << "\n-----------------------------------\n";
}

void PrintUserCard(sUser User)
{
    cout << "\nThe following are the user details:\n";
    cout << "-----------------------------------";
    cout << "\nUsername    : " << User.UserName;
    cout << "\nPassword    : " << User.Password;
    cout << "\nPermissions : " << User.Permissions;
    cout << "\n-----------------------------------\n";
}

bool FindClientByAccountNumber(string AccountNumber, vector <sClient> vClients, sClient& Client)
{
    for (sClient C : vClients)
    {
        if (C.AccountNumber == AccountNumber)
        {
            Client = C;
            return true;
        }
    }
    return false;
}

bool FindUserByUsername(string Username, vector<sUser>vUsers, sUser& User)
{
    for (sUser U : vUsers)
    {
        if (U.UserName == Username)
        {
            User = U;
            return true;
        }
    }
    return false;
}

sClient ChangeClientRecord(string AccountNumber)
{
    sClient Client;

    Client.AccountNumber = AccountNumber;

    cout << "\n\nEnter PinCode? ";
    getline(cin >> ws, Client.PinCode);

    cout << "Enter Name? ";
    getline(cin, Client.Name);

    cout << "Enter Phone? ";
    getline(cin, Client.Phone);

    cout << "Enter AccountBalance? ";
    cin >> Client.AccountBalance;
    return Client;
}

sUser ChangeUserRecord(string Username)
{
    sUser User;
    User.UserName = Username;

    cout << "\n\nEnter Password? ";
    getline(cin >> ws, User.Password);
 
    User.Permissions = static_cast<short>(GiveAccessToPermissions());
    return User;
}

bool MarkClientForDeleteByAccountNumber(string AccountNumber, vector <sClient>& vClients)
{
    for (sClient& C : vClients)
    {
        if (C.AccountNumber == AccountNumber)
        {
            C.MarkForDelete = true;
            return true;
        }
    }
    return false;
}

bool MarkUserForDeleteByUsername(string Username, vector <sUser>& vUsers)
{
    for (sUser& U : vUsers)
    {
        if (U.UserName == Username)
        {
            U.MarkForDelete = true;
            return true;
        }
    }
    return false;
}

void SaveCleintsDataToFile(string FileName, vector <sClient> vClients)
{
    fstream MyFile;
    MyFile.open(FileName, ios::out); 

    string DataLine;

    if (MyFile.is_open())
    {
        for (sClient C : vClients)
        {
            if (C.MarkForDelete == false)
            {
                 DataLine = ConvertRecordToLine_Clients(C);
                MyFile << DataLine << endl;
            }
        }
        MyFile.close();
    }
 }

void SaveUsersDataToFile(string FileName, vector <sUser> vUsers)
{
    fstream MyFile;
    MyFile.open(FileName, ios::out);

    string DataLine;

    if (MyFile.is_open())
    {
        for (sUser U : vUsers)
        {
            if (U.MarkForDelete == false)
            {
                DataLine = ConvertRecordToLine_Users(U);
                MyFile << DataLine << endl;
            }
        }
        MyFile.close();
    }
}

void AddDataLineToFile(string FileName, string  stDataLine)
{
    fstream MyFile;
    MyFile.open(FileName, ios::out | ios::app);

    if (MyFile.is_open())
    {
        MyFile << stDataLine << endl;
        MyFile.close();
    }
}

void AddNewClient()
{
    sClient Client;
    Client = ReadNewClient();
    AddDataLineToFile(ClientsFileName, ConvertRecordToLine_Clients(Client));
}

void AddNewUser()
{
    sUser User;
    User = ReadNewUser();
    AddDataLineToFile(UsersFileName, ConvertRecordToLine_Users(User));
}

void AddNewClients()
{
    char AddMore = 'Y';
    do
    {
        cout << "Adding New Client:\n\n";

        AddNewClient();
        cout << "\nClient Added Successfully, do you want to add more clients? Y/N? ";
        cin >> AddMore;

    } while (toupper(AddMore) == 'Y');
}

void AddNewUsers()
{
    char AddMore = 'Y';
    do
    {
        cout << "Adding New User:\n\n";
        AddNewUser();

        cout << "\nUser Added Successfully, do you want to add more users? Y/N? ";
        cin >> AddMore;

    } while (toupper(AddMore) == 'Y');
}

bool DeleteClientByAccountNumber(string AccountNumber, vector <sClient>& vClients)
{
    sClient Client;
    char Answer = 'n';

    if (FindClientByAccountNumber(AccountNumber, vClients, Client))
    {
        PrintClientCard(Client);

        cout << "\n\nAre you sure you want delete this client? y/n ? ";
        cin >> Answer;

        if (Answer == 'y' || Answer == 'Y')
        {
            MarkClientForDeleteByAccountNumber(AccountNumber, vClients);
            SaveCleintsDataToFile(ClientsFileName, vClients);

            cout << "\n\nClient Deleted Successfully.";
            return true;
        }
    }
    else
    {
        cout << "\nClient with Account Number (" << AccountNumber << ") is Not Found!";
        return false;
    }
}

bool DeleteUserByUsername(string Username, vector <sUser>& vUsers)
{
    sUser User;
    char Answer = 'n';

    if (FindUserByUsername(Username, vUsers, User))
    {
        PrintUserCard(User);

        cout << "\n\nAre you sure you want delete this user? y/n ? ";
        cin >> Answer;
        if (Answer == 'y' || Answer == 'Y')
        {
            MarkUserForDeleteByUsername(Username, vUsers);
            SaveUsersDataToFile(UsersFileName, vUsers);

            cout << "\n\nUser Deleted Successfully.";
            return true;
        }
    }
    else
    {
        cout << "\nUser with Username (" << Username << ") is Not Found!";
        return false;
    }
}

bool UpdateClientByAccountNumber(string AccountNumber, vector <sClient>& vClients)
{
    sClient Client;
    char Answer = 'n';

    if (FindClientByAccountNumber(AccountNumber, vClients, Client))
    {
        PrintClientCard(Client);

        cout << "\n\nAre you sure you want update this client? y/n ? ";
        cin >> Answer;
        if (Answer == 'y' || Answer == 'Y')
        {
            for (sClient& C : vClients)
            {
                if (C.AccountNumber == AccountNumber)
                {
                    C = ChangeClientRecord(AccountNumber);
                    break;
                }
            }
            SaveCleintsDataToFile(ClientsFileName, vClients);

            cout << "\n\nClient Updated Successfully.";
            return true;
        }
    }
    else
    {
        cout << "\nClient with Account Number (" << AccountNumber << ") is Not Found!";
        return false;
    }
}

bool UpdateUserByUsername(string Username, vector <sUser>& vUsers)
{
    sUser User;
    char Answer = 'n';

    if (FindUserByUsername(Username, vUsers, User))
    {
        PrintUserCard(User);

        cout << "\n\nAre you sure you want update this user? y/n ? ";
        cin >> Answer;
        if (Answer == 'y' || Answer == 'Y')
        {
            for (sUser& U : vUsers)
            {
                if (U.UserName == Username)
                {
                    U = ChangeUserRecord(Username);
                    break;
                }
            }
            SaveUsersDataToFile(UsersFileName, vUsers);

            cout << "\n\nUser Updated Successfully.";
            return true;
        }
    }
    else
    {
        cout << "\nUser with Username (" << Username << ") is Not Found!";
        return false;
    }
}

bool DepositBalanceToClientByAccountNumber(string AccountNumber, double Amount, vector <sClient>& vClients)
{
    char Answer = 'n';
    cout << "\n\nAre you sure you want perfrom this transaction? y/n ? ";
    cin >> Answer;
    if (Answer == 'y' || Answer == 'Y')
    {
        for (sClient& C : vClients)
        {
            if (C.AccountNumber == AccountNumber)
            {
                C.AccountBalance += Amount;
                SaveCleintsDataToFile(ClientsFileName, vClients);
                cout << "\n\nDone Successfully. New balance is: " << C.AccountBalance;

                return true;
            }
        }
    }
    return false;
}

string ReadClientAccountNumber()
{
    string AccountNumber = "";

    cout << "\nPlease enter AccountNumber? ";
    cin >> AccountNumber;
    return AccountNumber;
}

string ReadUsername()
{
    string Username = "";

    cout << "\nPlease enter Username? ";
    cin >> Username;
    return Username;
}

void ShowDeleteClientScreen()
{
    cout << "\n-----------------------------------\n";
    cout << "\tDelete Clients Screen";
    cout << "\n-----------------------------------\n";

    vector <sClient> vClients = LoadClientsDataFromFile(ClientsFileName);
    string AccountNumber = ReadClientAccountNumber();
    DeleteClientByAccountNumber(AccountNumber, vClients);
}

void ShowDeleteUserScreen()
{
    cout << "\n-----------------------------------\n";
    cout << "\tDelete Users Screen";
    cout << "\n-----------------------------------\n";

    vector<sUser>vUsers = LoadUsersDataFromFile(UsersFileName);
    string Username = ReadUsername();
    DeleteUserByUsername(Username, vUsers);
}

void ShowUpdateClientScreen()
{
    cout << "\n-----------------------------------\n";
    cout << "\tUpdate Client Info Screen";
    cout << "\n-----------------------------------\n";

    vector <sClient> vClients = LoadClientsDataFromFile(ClientsFileName);
    string AccountNumber = ReadClientAccountNumber();
    UpdateClientByAccountNumber(AccountNumber, vClients);
}

void ShowUpdateUserScreen()
{
    cout << "\n-----------------------------------\n";
    cout << "\tUpdate User Info Screen";
    cout << "\n-----------------------------------\n";

    vector <sUser> vUsers = LoadUsersDataFromFile(UsersFileName);
    string Username = ReadUsername();
    UpdateUserByUsername(Username, vUsers);
}

void ShowAddNewClientsScreen()
{
    cout << "\n-----------------------------------\n";
    cout << "\tAdd New Clients Screen";
    cout << "\n-----------------------------------\n";

    AddNewClients();
}

void ShowAddNewUsersScreen()
{
    cout << "\n-----------------------------------\n";
    cout << "\tAdd New Users Screen";
    cout << "\n-----------------------------------\n";

    AddNewUsers();
}

void ShowFindClientScreen()
{
    cout << "\n-----------------------------------\n";
    cout << "\tFind Client Screen";
    cout << "\n-----------------------------------\n";

    vector <sClient> vClients = LoadClientsDataFromFile(ClientsFileName);
    sClient Client;
    string AccountNumber = ReadClientAccountNumber();
    if (FindClientByAccountNumber(AccountNumber, vClients, Client))
        PrintClientCard(Client);
    else
        cout << "\nClient with Account Number[" << AccountNumber << "] is not found!";
}

void ShowFindUserScreen()
{
    cout << "\n-----------------------------------\n";
    cout << "\tFind User Screen";
    cout << "\n-----------------------------------\n";

    vector <sUser> vUsers = LoadUsersDataFromFile(UsersFileName);
    sUser User;
    string Username = ReadUsername();
    if (FindUserByUsername(Username, vUsers, User))
        PrintUserCard(User);
    else
        cout << "\nUser with Account Number [" << Username << "] is not found!";
}

void ShowDepositScreen()
{
    cout << "\n-----------------------------------\n";
    cout << "\tDeposit Screen";
    cout << "\n-----------------------------------\n";


    sClient Client;

    vector <sClient> vClients = LoadClientsDataFromFile(ClientsFileName);
    string AccountNumber = ReadClientAccountNumber();


    while (!FindClientByAccountNumber(AccountNumber, vClients, Client))
    {
        cout << "\nClient with [" << AccountNumber << "] does not exist.\n";
        AccountNumber = ReadClientAccountNumber();
    }


    PrintClientCard(Client);

    double Amount = 0;
    cout << "\nPlease enter deposit amount? ";
    cin >> Amount;

    DepositBalanceToClientByAccountNumber(AccountNumber, Amount, vClients);

}

void ShowWithDrawScreen()
{
    cout << "\n-----------------------------------\n";
    cout << "\tWithdraw Screen";
    cout << "\n-----------------------------------\n";

    sClient Client;

    vector <sClient> vClients = LoadClientsDataFromFile(ClientsFileName);
    string AccountNumber = ReadClientAccountNumber();


    while (!FindClientByAccountNumber(AccountNumber, vClients, Client))
    {
        cout << "\nClient with [" << AccountNumber << "] does not exist.\n";
        AccountNumber = ReadClientAccountNumber();
    }

    PrintClientCard(Client);

    double Amount = 0;
    cout << "\nPlease enter withdraw amount? ";
    cin >> Amount;

     while (Amount > Client.AccountBalance)
    {
        cout << "\nAmount Exceeds the balance, you can withdraw up to : " << Client.AccountBalance << endl;
        cout << "Please enter another amount? ";
        cin >> Amount;
    }

     DepositBalanceToClientByAccountNumber(AccountNumber, Amount * -1, vClients);
}

void ShowTotalBalancesScreen()
{
    ShowTotalBalances();
}

void ShowListUsers()
{
    vector<sUser>vUsers = LoadUsersDataFromFile(UsersFileName);
    cout << "\t\t\t\tUsers List " << vUsers.size() << " User(s)" << endl;
    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;
    cout << "| " << left << setw(15) << "User Name";
    cout << "| " << left << setw(12) << "Password";
    cout << "| " << left << setw(12) << "permissions";
    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;

    if (vUsers.size() == 0)
        cout << "\t\t\t\tThere Are No Users " << endl;
    else
        for (sUser U : vUsers)
            PrintUserRecordLine(U);

    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;
}
 
void GoBackToMainMenue()
{
    cout << "\n\nPress any key to go back to Main Menue...";
    system("pause>0");
    ShowMainMenue();
}

void GoBackToTransactionsMenue()
{
    cout << "\n\nPress any key to go back to Transactions Menue...";
    system("pause>0");
    ShowTransactionsMenue();
}

void GoBackToManageUsersMenue()
{
    cout << "\n\nPress any key to go back to Manage Users Menue...";
    system("pause>0");
    ShowManageUsersMenueScreen();
}

short ReadOption(short NumberOfOptions)
{
    cout << "Choose what do you want to do? [1 to " << NumberOfOptions << "] ? ";
    short Choice = 0;
    cin >> Choice;

    return Choice;
}

void PerformTranactionsMenueOption(enTransactionsMenueOptions TransactionMenueOption)
{
    switch (TransactionMenueOption)
    {
    case enTransactionsMenueOptions::eDeposit:
        system("cls");
        ShowDepositScreen();
        GoBackToTransactionsMenue();
        break;
    
    case enTransactionsMenueOptions::eWithdraw:
        system("cls");
        ShowWithDrawScreen();
        GoBackToTransactionsMenue();
        break;
 
    case enTransactionsMenueOptions::eShowTotalBalance:
        system("cls");
        ShowTotalBalancesScreen();
        GoBackToTransactionsMenue();
        break;
 
    case enTransactionsMenueOptions::eShowMainMenue:
        ShowMainMenue();
    }
}

void ShowTransactionsMenue()
{
    system("cls");
    cout << "===========================================\n";
    cout << "\t\tTransactions Menue Screen\n";
    cout << "===========================================\n";
    cout << "\t[1] Deposit.\n";
    cout << "\t[2] Withdraw.\n";
    cout << "\t[3] Total Balances.\n";
    cout << "\t[4] Main Menue.\n";
    cout << "===========================================\n";
    PerformTranactionsMenueOption(static_cast<enTransactionsMenueOptions>(ReadOption(4)));
}

void PerformManageUsersMenueOptions(enManageUsersMenueOptions ManageUsersMenueOption)
{
    switch (ManageUsersMenueOption)
    {
    case enManageUsersMenueOptions::eListUsers:
        system("cls");
        ShowListUsers();
        GoBackToManageUsersMenue();
        break;

    case enManageUsersMenueOptions::eAddUser:
        system("cls");
        ShowAddNewUsersScreen();
        GoBackToManageUsersMenue();
        break;

    case enManageUsersMenueOptions::eDeleteUser:
        system("cls");
        ShowDeleteUserScreen();
        GoBackToManageUsersMenue();
        break;

    case enManageUsersMenueOptions::eUpdateUser:
        system("cls");
        ShowUpdateUserScreen();
        GoBackToManageUsersMenue();
        break;

    case enManageUsersMenueOptions::eFindUser:
        system("cls");
        ShowFindUserScreen();
        GoBackToManageUsersMenue();
        break;

    case enManageUsersMenueOptions::eMainMenue:
        ShowMainMenue();
    }
}

void ShowManageUsersMenueScreen()
{
    system("cls");
    cout << "===========================================\n";
    cout << "\tManage Users Menue Screen\n";
    cout << "===========================================\n";
    cout << "\t[1] List Users.\n";
    cout << "\t[2] Add New User.\n";
    cout << "\t[3] Delete User.\n";
    cout << "\t[4] Update User.\n";
    cout << "\t[5] Find User.\n";
    cout << "\t[6] Main Menue.\n";
    cout << "===========================================\n";
    PerformManageUsersMenueOptions(static_cast<enManageUsersMenueOptions>(ReadOption(6)));
}

bool CheckUserPermissions(short Permissions,enPermissions Permission)
{
    if (Permissions & Permission)
    {
        return true;
    }
    return false;
}

void PerformMainMenueOption(enMainMenueOptions MainMenueOption)
{
    switch (MainMenueOption)
    {
    case enMainMenueOptions::eListClients:
        if (CheckUserPermissions(Permissions, enPermissions::eListClients_))
        {
            system("cls");
            ShowAllClientsScreen();
            GoBackToMainMenue();
        }
        else
        {
            system("cls");
            cout << "\n------------------------------------------";
            cout << "\nAccess Denied,\n" << "You do not have Permission to do this,\n" << "Please conact your Admin.\n";
            cout << "------------------------------------------";
            GoBackToMainMenue();
        }
        break;
  
    case enMainMenueOptions::eAddNewClient:
        if (CheckUserPermissions(Permissions, enPermissions::eAddNewClient_))
        {
            system("cls");
            ShowAddNewClientsScreen();
            GoBackToMainMenue();
        }
        else
        {
            system("cls");
            cout << "\n------------------------------------------";
            cout << "\nAccess Denied,\n" << "You do not have Permission to do this,\n" << "Please conact your Admin.\n";
            cout << "------------------------------------------";
            GoBackToMainMenue();
        }
        break;

    case enMainMenueOptions::eDeleteClient:
        if (CheckUserPermissions(Permissions, enPermissions::eDeleteClient_))
        {
            system("cls");
            ShowDeleteClientScreen();
            GoBackToMainMenue();
        }
        else
        {
            system("cls");
            cout << "\n------------------------------------------";
            cout << "\nAccess Denied,\n" << "You do not have Permission to do this,\n" << "Please conact your Admin.\n";
            cout << "------------------------------------------";  
            GoBackToMainMenue();
        }
        break;

    case enMainMenueOptions::eUpdateClient:
        if (CheckUserPermissions(Permissions, enPermissions::eUpdateClient_))
        {
            system("cls");
            ShowUpdateClientScreen();
            GoBackToMainMenue();
        }
        else
        {
            system("cls");
            cout << "\n------------------------------------------";
            cout << "\nAccess Denied,\n" << "You do not have Permission to do this,\n" << "Please conact your Admin.\n";
            cout << "------------------------------------------";
            GoBackToMainMenue();
        }
        break;

    case enMainMenueOptions::eFindClient:
        if (CheckUserPermissions(Permissions, enPermissions::eFindClient_))
        {
            system("cls");
            ShowFindClientScreen();
            GoBackToMainMenue();
        }
        else
        {
            system("cls");
            cout << "\n------------------------------------------";
            cout << "\nAccess Denied,\n" << "You do not have Permission to do this,\n" << "Please conact your Admin.\n";
            cout << "------------------------------------------";
            GoBackToMainMenue();
        }
        break;

    case enMainMenueOptions::eShowTransactionsMenue:
        if (CheckUserPermissions(Permissions, enPermissions::eShowTransactionsMenue_))
        {
            system("cls");
            ShowTransactionsMenue();
        }
        else
        {
            system("cls");
            cout << "\n------------------------------------------";
            cout << "\nAccess Denied,\n" << "You do not have Permission to do this,\n" << "Please conact your Admin.\n";
            cout << "------------------------------------------";
            GoBackToMainMenue();
        }
        break;

    case enMainMenueOptions::eManageUsers:
        if (CheckUserPermissions(Permissions, enPermissions::eFindClient_))
        {
            system("cls");
            ShowManageUsersMenueScreen();
        }
        else
        {
            system("cls");
            cout << "\n------------------------------------------";
            cout << "\nAccess Denied,\n" << "You do not have Permission to do this,\n" << "Please conact your Admin.\n";
            cout << "------------------------------------------";
            GoBackToMainMenue();
        }
        break;

    case enMainMenueOptions::eLogout:
        system("cls");
        Login();
    }
}

void ShowMainMenue()
{
    system("cls");
    cout << "===========================================\n";
    cout << "\t\tMain Menue Screen\n";
    cout << "===========================================\n";
    cout << "\t[1] Show Client List.\n";
    cout << "\t[2] Add New Client.\n";
    cout << "\t[3] Delete Client.\n";
    cout << "\t[4] Update Client Info.\n";
    cout << "\t[5] Find Client.\n";
    cout << "\t[6] Transactions.\n";
    cout << "\t[7] Manage Users.\n";
    cout << "\t[8] Logout.\n";
    cout << "===========================================\n";
    PerformMainMenueOption(static_cast<enMainMenueOptions>(ReadOption(8)));
}

bool CheckLoginOperationByMatchUsernameAndPassword(string UserName, string Password,short& Permissions,string FileName)
{
    fstream MyFile;
    MyFile.open(FileName, ios::in);
    
    if (MyFile.is_open())
    {
        string Line;
        sUser User;

        while (getline(MyFile, Line))
        {
            User = ConvertLinetoRecord_Users(Line);
            if (User.UserName == UserName && User.Password == Password)
            {
                Permissions = User.Permissions;
                 MyFile.close();
                return true;
            }
        }
        MyFile.close();
        return false;
    }
}

void ShowLoginScreen()
{
    cout << "\n--------------------------------------\n";
    cout << "\tLogin Screen";
    cout << "\n--------------------------------------\n";
}

void PerformLoginOperation()
{
    sUser User;

    cout << "Enter Username? ";
    cin >> User.UserName;
    cout << "Enter Password? ";
    cin >> User.Password;

    while (!CheckLoginOperationByMatchUsernameAndPassword(User.UserName, User.Password, User.Permissions, UsersFileName))
    {
        system("cls");
        ShowLoginScreen();
        cout << "Invalid Username/Password!\n";
        cout << "Enter Username? ";
        cin >> User.UserName;
        cout << "Enter Password? ";
        cin >> User.Password;
    }
    Permissions = User.Permissions;
    ShowMainMenue();
}

void Login()
{
    ShowLoginScreen();
    PerformLoginOperation();
}

int main()
{
     Login();
    system("pause>0");
    return 0;
}
