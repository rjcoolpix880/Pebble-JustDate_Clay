module.exports = [
  {
    "type": "heading",
    "defaultValue": "Just Date Settings"
  },
  {
    "type": "text",
    "defaultValue": "Make that change."
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Colors"
      },
      {
        "type": "color",
        "messageKey": "BackgroundColor",
        "defaultValue": "0x000000",
        "label": "Background Color"
      },
      {
        "type": "color",
        "messageKey": "ForegroundColor",
        "defaultValue": "0xFFFFFF",
        "label": "Text Color"
      }
    ]
  },
  
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Month Display Setting"
      },
      
            {
        "type": "toggle",
        "messageKey": "ShowMonth",
        "label": "Hide / Show Month",
        "defaultValue": false
      },
      
      
      {
        "type": "toggle",
        "messageKey": "MonthConfig",
        "label": "Month as Number / Name",
        "defaultValue": false
      },
      
      
   
 
    
      
      
    ]
  },
  
  
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];