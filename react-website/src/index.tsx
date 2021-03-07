import React from 'react';
import ReactDOM from 'react-dom';
import './index.css';
import { App } from './App';
import { createMuiTheme, CssBaseline, ThemeProvider } from "@material-ui/core";

const theme = createMuiTheme({
  palette: {
    primary: {
      main: "#18ab29"
    },
    secondary: {
      main: "#2f6627"
    },
    background: {
      default: "#121212"
    }
  }
});


ReactDOM.render(
  <React.StrictMode>
    <ThemeProvider theme={theme}>
      <CssBaseline />
      <App />
    </ThemeProvider>
  </React.StrictMode>,
  document.getElementById('root')
);


