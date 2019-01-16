package com.springboot.entities;

import javax.persistence.*;
import java.util.Date;

/**
 * Created by 默默 on 2017/5/8.
 */
@Entity
@Table(name = "wendu")
public class Wendu {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Integer id;
    private Double wendu;
    private Double shidu;
    private Date time;

    public Double getShidu() {
        return shidu;
    }

    public void setShidu(Double shidu) {
        this.shidu = shidu;
    }

    public Double getWendu() {
        return wendu;
    }

    public void setWendu(Double wendu) {
        this.wendu = wendu;
    }

    public Integer getId() {
        return id;
    }

    public void setId(Integer id) {
        this.id = id;
    }



    public Date getTime() {
        return time;
    }

    public void setTime(Date time) {
        this.time = time;
    }
}
