module.exports = [
  {
    type: "heading",
    defaultValue: "UQM Configuration",
  },
  {
    type: "section",
    items: [
      {
        type: "heading",
        defaultValue: "Watch Settings",
      },
      {
        type: "toggle",
        messageKey: "Hires",
        label: "Hires Captains",
        defaultValue: true,
      },
      {
        type: "select",
        messageKey: "PilotChange",
        label: "Change pilot every",
        defaultValue: "5",
        options: [
          {
            label: "Never",
            value: "0",
          },
          {
            label: "Minute",
            value: "1",
          },
          {
            label: "5 Minutes",
            value: "5",
          },
          {
            label: "10 Minutes",
            value: "10",
          },
          {
            label: "60 Minutes",
            value: "60",
          },
        ],
      },
      {
        type: "select",
        messageKey: "CapChange",
        label: "Change captain every",
        defaultValue: "5",
        options: [
          {
            label: "Never",
            value: "0",
          },
          {
            label: "Minute",
            value: "1",
          },
          {
            label: "5 Minutes",
            value: "5",
          },
          {
            label: "10 Minutes",
            value: "10",
          },
          {
            label: "60 Minutes",
            value: "60",
          },
        ],
      },
    ],
  },
  {
    type: "submit",
    defaultValue: "Save Settings",
  },
];
