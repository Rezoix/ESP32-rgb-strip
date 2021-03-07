import { Slider } from "@material-ui/core";
import axios from "axios";
import React, { FunctionComponent } from "react";
import { throttle } from "../util";

interface SliderProps {
  value: number;
  setValue: React.Dispatch<React.SetStateAction<number>>;
  range: number[];
  step: number;
  type: string;
}

export const CustomSlider: FunctionComponent<SliderProps> = (props: SliderProps) => {
  const onChange = (event: any, value: any) => {
    const url = "/set?" + props.type + "=" + value;

    axios.post(url).catch((e) => {
      console.log("Error: Failed to change mode");
      //console.log(e);
      return;
    });

    props.setValue(value);
  };

  const throttledOnChange = throttle((event: any, value: any) => onChange(event, value));

  return (
    <>
      <Slider
        value={props.value}
        valueLabelDisplay="auto"
        min={props.range[0]}
        max={props.range[1]}
        step={props.step}
        onChange={throttledOnChange}
      />
    </>
  );
};
