﻿using demo.Classes;
using System;
using System.Windows;
using System.Windows.Controls;

namespace demo.Controls.View
{
    /// <summary>
    /// Логика взаимодействия для ControlViewStoredValues.xaml
    /// </summary>
    public partial class ControlViewStoredValues : UserControl
    {
        public static readonly DependencyProperty MemoryDataProperty;
        public static readonly DependencyProperty KeyProperty;
        public static readonly DependencyProperty ValueProperty;

        static ControlViewStoredValues()
        {
            MemoryDataProperty = DependencyProperty.Register("MemoryData",
                typeof(StoredRecordsModel),
                typeof(ControlViewStoredValues));

            KeyProperty = DependencyProperty.Register("Key",
                typeof(UInt16),
                typeof(ControlViewStoredValues),
                new PropertyMetadata(
                    (UInt16)0,
                    new PropertyChangedCallback(KeyChangedCallback),
                    new CoerceValueCallback(KeyCoerceValueCallback)));

            ValueProperty = DependencyProperty.Register("Value",
                typeof(UInt16),
                typeof(ControlViewStoredValues),
                new PropertyMetadata(
                    (UInt16)0,
                    new PropertyChangedCallback(ValueChangedCallback),
                    new CoerceValueCallback(ValueCoerceValueCallback)));
        }

        #region Key callback

        private static void KeyChangedCallback(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
        }

        private static object KeyCoerceValueCallback(DependencyObject d, object baseValue)
        {
            UInt16 key = Convert.ToUInt16(baseValue);
            return Convert.ToUInt16(Math.Min(key, 65535u));
        }

        #endregion

        #region Value callback

        private static void ValueChangedCallback(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
        }

        private static object ValueCoerceValueCallback(DependencyObject d, object baseValue)
        {
            UInt16 value = Convert.ToUInt16(baseValue);
            return Convert.ToUInt16(Math.Min(value, 65535u));
        }

        #endregion

        private static void MemoryDataChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
        }

        public ControlViewStoredValues()
        {
            InitializeComponent();
            (this.Content as FrameworkElement).DataContext = this;
        }

        public StoredRecordsModel MemoryData
        {
            get
            {
                return (StoredRecordsModel)this.GetValue(MemoryDataProperty);
            }
            set
            {
                this.SetValue(MemoryDataProperty, value);
            }
        }

        public UInt16 Key
        {
            get
            {
                return Convert.ToUInt16(this.GetValue(KeyProperty));
            }
            set
            {
                this.SetValue(KeyProperty, value);
            }
        }

        public UInt16 Value
        {
            get
            {
                return Convert.ToUInt16(this.GetValue(ValueProperty));
            }
            set
            {
                this.SetValue(ValueProperty, value);
            }
        }

        private void ListBoxStoredValues_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (e.AddedItems.Count > 0)
            {
                StoredRecord record = e.AddedItems[0] as StoredRecord;
                this.Key = record.Key;
                this.Value = record.Value;
            }
        }

        private void ButtonStoreValue_Click(object sender, RoutedEventArgs e)
        {
            Commands.StoreValue.Execute(new Tuple<UInt16, UInt16>(this.Key, this.Value), (Button)sender);
        }
    }
}
