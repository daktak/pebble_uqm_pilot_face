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
        messageKey: "PilotSelect",
        label: "Pilot race",
        defaultValue: "0",
        options: [
          { label: "Random", value: "0" },
          { label: "Spathi", value: "1" },
          { label: "Androsynth", value: "2" },
          { label: "Arilou", value: "3" },
          { label: "Chenjesu", value: "4" },
          { label: "Chmmr", value: "5" },
          { label: "Druuge", value: "6" },
          { label: "Human", value: "7" },
          { label: "Ilwrath", value: "8" },
          { label: "Kohr-Ah", value: "9" },
          { label: "Melnorme", value: "10" },
          { label: "Mmrnmhrm", value: "11" },
          { label: "Mycon", value: "12" },
          { label: "Orz", value: "13" },
          { label: "Pkunk", value: "14" },
          { label: "Shofixti", value: "15" },
          { label: "Slylandro", value: "16" },
          { label: "Supox", value: "17" },
          { label: "Syreen", value: "18" },
          { label: "Thraddash", value: "19" },
          { label: "Umgah", value: "20" },
          { label: "Ur-Quan", value: "21" },
          { label: "Utwig", value: "22" },
          { label: "Vux", value: "23" },
          { label: "Yehat", value: "24" },
          { label: "Zoq-Fot-Pik", value: "25" },
        ],
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
    type: "section",
    items: [
      {
        type: "heading",
        defaultValue: "Quiet Time Settings",
      },
      {
        type: "toggle",
        messageKey: "PilotQuietTime",
        label: "Don't change pilot/captain at these times",
        defaultValue: false,
      },
      {
        type: "slider",
        messageKey: "PilotQuietStart",
        defaultValue: 23,
        label: "Changes stop at (24hr)",
        min: 0,
        max: 23,
      },
      {
        type: "slider",
        messageKey: "PilotQuietStop",
        defaultValue: 6,
        label: "Changes resume at (24hr)",
        min: 0,
        max: 23,
      },
    ],
  },
  {
    type: "submit",
    defaultValue: "Save Settings",
  },
];
