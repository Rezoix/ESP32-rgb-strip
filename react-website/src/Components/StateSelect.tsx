import { Button } from "@material-ui/core";
import axios from "axios";
import React, { FunctionComponent } from "react";

interface ModeProps {
  buttonMode: string;
  mode: string;
  setMode: React.Dispatch<React.SetStateAction<string>>;
}

export const StateSelect: FunctionComponent<ModeProps> = (props: ModeProps) => {
  const onClick = () => {
    const url = "/set?mode=" + props.buttonMode;

    axios.post(url).catch((e) => {
      console.log("Error: Failed to change mode");
      //console.log(e);
      return;
    });

    props.setMode(props.buttonMode);
  };

  return (
    <>
      <Button
        variant="contained"
        color={props.mode === props.buttonMode ? "primary" : "secondary"}
        onClick={onClick}
      >
        {props.buttonMode}
      </Button>
    </>
  );
};
