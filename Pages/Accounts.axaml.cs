using System;
using System.Collections.Generic;
using Avalonia;
using Avalonia.Controls;
using Avalonia.VisualTree;
using HPM_Companion.Pages.Components;

namespace HPM_Companion.Pages;

public partial class Accounts : UserControl
{
    //This will probably become an object, just want
    //to have a structure for the dummy data right now
    private struct UserAccData(string login, string password, string service)
    {
        public readonly string Login = login, Password = password, Service = service;
    }
    
    private const int GridCols = 2;
    private readonly List<UserAccData> _accountList =
    [
        new("google@email.com", "password1", "Google"),
        new("facebook@email.com", "password2", "Facebook"),
        new("amazon@email.com", "password3", "Amazon"),
        new("uber@email.com", "password4", "Uber"),
        new("instagram@email.com", "password5", "Instagram")
    ];
    public Accounts()
    {
        InitializeComponent();
        AttachedToVisualTree += OnAttachedToVisualTree;
    }

    private void CreateAccountCards()
    {
        for (var i = 0; i < GridCols; i++)
        {
            AccountCards.ColumnDefinitions.Add(new ColumnDefinition(GridLength.Auto));
        }
        var rows = (int)Math.Ceiling((double) _accountList.Count / GridCols);
        for (int i = 0; i < rows; i++)
        {
            AccountCards.RowDefinitions.Add(new RowDefinition(GridLength.Auto));
        }

        var window = this.GetVisualRoot();
        
        double windowWidth = window?.ClientSize.Width ?? 0; 
        for (var i = 0; i < _accountList.Count; i++)
        {
            var card = new AccountCard();
            card.MaxWidth = 500;
            card.Width = windowWidth / 2;
            card.AccountName.Text = _accountList[i].Service;
            card.UsernameInput.Text = _accountList[i].Login;
            card.PasswordInput.Text = _accountList[i].Password;
            Grid.SetColumn(card, i % GridCols);
            Grid.SetRow(card, (int)Math.Floor(i / (double) GridCols));
            AccountCards.Children.Add(card);
        }
    }
    
    private void OnAttachedToVisualTree(object? sender, VisualTreeAttachmentEventArgs e)
    {
        CreateAccountCards();
    }
}